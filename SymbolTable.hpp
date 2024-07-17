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
    SymbolTable() : whileLoopCount(0) {
        enterScope();  // Start with global scope
        // Add library functions
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
                output::printID(entry.second.name, 0, output::makeFunctionType(entry.second.type, entry.second.params));
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
        tables.back()[name] = {name, returnType, 0, true, params};
        return true;
    }

    bool isSymbolDefined(const std::string& name) const {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            if (it->find(name) != it->end()) {
                return true;
            }
        }
        return false;
    }

    bool isSymbolDefinedInCurrentScope(const std::string& name) const {
        return tables.back().find(name) != tables.back().end();
    }

    std::string getSymbolType(const std::string& name) const {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return found->second.type;
            }
        }
        return "";
    }

    bool isFunction(const std::string& name) const {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return found->second.isFunction;
            }
        }
        return false;
    }

    void enterWhileLoop() { whileLoopCount++; }
    void exitWhileLoop() { whileLoopCount--; }
    bool isInsideWhileLoop() const { return whileLoopCount > 0; }

    bool isValidAssignment(const std::string& lhs, const std::string& rhs) const {
        if (lhs == rhs) return true;
        if (lhs == "INT" && rhs == "BYTE") return true;
        return false;
    }

    bool isNumericType(const std::string& type) const {
        return type == "INT" || type == "BYTE";
    }

    bool isBoolType(const std::string& type) const {
        return type == "BOOL";
    }

    std::string getLargerType(const std::string& type1, const std::string& type2) const {
        if (type1 == "INT" || type2 == "INT") return "INT";
        return "BYTE";
    }

    bool isValidFunctionCall(const std::string& funcName, const std::string& argType) const {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            auto found = it->find(funcName);
            if (found != it->end() && found->second.isFunction) {
                if (found->second.params.empty()) return false;
                return isValidAssignment(found->second.params[0], argType);
            }
        }
        return false;
    }

    std::string getFunctionReturnType(const std::string& funcName) const {
        return getSymbolType(funcName);
    }

    std::vector<std::string> getFunctionParams(const std::string& funcName) const {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            auto found = it->find(funcName);
            if (found != it->end() && found->second.isFunction) {
                return found->second.params;
            }
        }
        return {};
    }
};

#endif // SYMBOL_TABLE_HPP