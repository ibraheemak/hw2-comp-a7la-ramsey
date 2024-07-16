#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "output.hpp"

class SymbolTable {
private:
    struct Symbol {
        std::string name;
        std::string type;
        int offset;
        bool isFunction;
        std::vector<std::string> params;
    };

    std::vector<std::unordered_map<std::string, Symbol>> tables;
    std::vector<int> offsets;
    int whileLoopCount;

public:
    bool isVariableDefined(const std::string& name) {
    return findSymbol(name) != nullptr;
}
    std::vector<std::string> getFunctionParams(const std::string& funcName) {
        Symbol* sym = findSymbol(funcName);
        return (sym && sym->isFunction) ? sym->params : std::vector<std::string>();
    }

    SymbolTable() : whileLoopCount(0) {
        enterScope();  // Start with global scope
        // Add library functions in the specified order
        addFunction("print", "VOID", {"STRING"});
        addFunction("printi", "VOID", {"INT"});
        addFunction("readi", "INT", {});
    }

    void enterScope() {
        tables.push_back(std::unordered_map<std::string, Symbol>());
        offsets.push_back(offsets.empty() ? 0 : offsets.back());
    }

    void exitScope() {
        output::endScope();
        for (const auto& entry : tables.back()) {
            if (entry.second.isFunction) {
                output::printID(entry.second.name, 0, entry.second.type);
            } else {
                output::printID(entry.second.name, entry.second.offset, entry.second.type);
            }
        }
        tables.pop_back();
        offsets.pop_back();
    }

    bool addSymbol(const std::string& name, const std::string& type) {
        if (isSymbolDefinedInCurrentScope(name)) {
            return false;
        }
        int offset = offsets.back()++;
        tables.back()[name] = {name, type, offset, false, {}};
        return true;
    }

    bool addFunction(const std::string& name, const std::string& returnType, const std::vector<std::string>& params) {
        if (isSymbolDefinedInCurrentScope(name)) {
            return false;
        }
        std::string funcType = output::makeFunctionType(params, returnType);
        tables.back()[name] = {name, funcType, 0, true, params};
        return true;
    }

    Symbol* findSymbol(const std::string& name) {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return &(found->second);
            }
        }
        return nullptr;
    }

    bool isSymbolDefinedInCurrentScope(const std::string& name) {
        return tables.back().find(name) != tables.back().end();
    }

    bool isFunction(const std::string& name) {
        Symbol* sym = findSymbol(name);
        return sym && sym->isFunction;
    }

    std::string getSymbolType(const std::string& name) {
        Symbol* sym = findSymbol(name);
        return sym ? sym->type : "";
    }

    int getCurrentOffset() {
        return offsets.back();
    }

    void enterWhileLoop() {
        whileLoopCount++;
    }

    void exitWhileLoop() {
        whileLoopCount--;
    }

    bool isInsideWhileLoop() {
        return whileLoopCount > 0;
    }

    bool isValidAssignment(const std::string& lhs, const std::string& rhs) {
        if (lhs == rhs) return true;
        if (lhs == "INT" && rhs == "BYTE") return true;
        return false;
    }

    bool checkValidCast(const std::string& fromType, const std::string& toType) {
        return (fromType == "INT" || fromType == "BYTE") && (toType == "INT" || toType == "BYTE");
    }

    bool isNumericType(const std::string& type) {
        return type == "INT" || type == "BYTE";
    }

    bool isBoolType(const std::string& type) {
        return type == "BOOL";
    }

    bool isStringType(const std::string& type) {
        return type == "STRING";
    }

    std::string getLargerType(const std::string& type1, const std::string& type2) {
        if (type1 == "INT" || type2 == "INT") return "INT";
        return "BYTE";
    }

    bool isValidByteLiteral(int value) {
        return value >= 0 && value <= 255;
    }

    bool isValidFunctionCall(const std::string& funcName, const std::string& argType) {
        Symbol* sym = findSymbol(funcName);
        if (!sym || !sym->isFunction) return false;
        if (funcName == "print") return isStringType(argType);
        if (funcName == "printi") return isNumericType(argType);
        if (funcName == "readi") return argType.empty();
        return false;  // FanC only has library functions
    }
    std::string getFunctionReturnType(const std::string& funcName) {
        Symbol* sym = findSymbol(funcName);
        if (!sym || !sym->isFunction) return "";
        size_t pos = sym->type.find("->");
        return (pos != std::string::npos) ? sym->type.substr(pos + 2) : "";
    }

    bool isValidRelop(const std::string& type1, const std::string& type2) {
        return isNumericType(type1) && isNumericType(type2);
    }

    bool isValidLogicalOp(const std::string& type1, const std::string& type2) {
        return isBoolType(type1) && isBoolType(type2);
    }

    bool isValidReturnStatement() {
        // FanC doesn't require return statements, so always valid
        return true;
    }
};

#endif // SYMBOL_TABLE_HPP