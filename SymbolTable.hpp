#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <vector>
#include <string>
#include "hw3_output.hpp"
#include <iostream>
using namespace std;
#include <algorithm>
#include <string>

inline std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::toupper(c); });
    return s;
}

// offset stack to keep track as in the tutorials - t6
class offsetStack {
private:
    vector<int> offset_Stack;

public:
    offsetStack() : offset_Stack(1, 0) {}  // Initialize with one element set to 0

    void push_offset(int x) {
        offset_Stack.push_back(x);
    }

    int pop_offset(int lineno) {
        if (offset_Stack.empty()) {
            output::errorMismatch(lineno);
            exit(0);
        }
        int x = offset_Stack.back();
        offset_Stack.pop_back();
        return x;
    }
    int top_offset(int lineno) const {
    if (offset_Stack.empty()) {
        output::errorMismatch(lineno);
        exit(0);
    }
    return offset_Stack.back();
}

    void pop_n(size_t n, int lineno) {
        if (n > offset_Stack.size()) {
            output::errorMismatch(lineno);
            exit(0);
        }
        offset_Stack.resize(offset_Stack.size() - n);
    }
    bool empty() const {
        return offset_Stack.empty();
    }
    void update_top_offset() {
        if (!offset_Stack.empty()) {
            offset_Stack.back()++;
        }
    }

    size_t size() const {
        return offset_Stack.size();
    }
};


////////////////////////////////////////





// !! check this
/// creating a table entry that will save local variables - t6
class tableEntry {
public:
    string name;
    string type;
    int offset;
    tableEntry(const string& name, const string& type, int offset)
        : name(name), type(type), offset(offset) {}
    virtual ~tableEntry() {} // Add a virtual destructor
};

class functions : public tableEntry {
public:
    vector<string> all_arg;
    string ret_type;
    bool isOverride;

    functions(const string& name, const string& type, int offset,
              vector<string> all_the_arg, string the_ret_type, bool isOverride)
        : tableEntry(name, "function", offset), all_arg(all_the_arg),
          ret_type(the_ret_type), isOverride(isOverride) {}
};

// scope entry for every new scope 
class ScopeBlock {
public:
    vector<tableEntry*> scope;
    ScopeBlock* parent;

    ScopeBlock() : parent(nullptr) {}
    ScopeBlock(ScopeBlock* parent) : parent(parent) {}
};

class TablesStack {
public:
    vector<ScopeBlock*> stackTable;
    ScopeBlock* ParentScope;

    TablesStack() {
        ParentScope = InsertTable(nullptr);
    }
    ~TablesStack() {
        for (auto scope : stackTable) {
            for (auto entry : scope->scope) {
                delete entry;
            }
            delete scope;
        }
    }
    ScopeBlock* InsertTable(ScopeBlock* parent) {
        ScopeBlock* newScope = new ScopeBlock(parent);
        stackTable.push_back(newScope);
        return newScope;
    }

    void insert(ScopeBlock* curScope, const string& name, const string& type, int offset) {
        tableEntry* newEntry = new tableEntry(name, type, offset);
        curScope->scope.push_back(newEntry);
    }
    // !! check this
    void insertFunction(ScopeBlock* curScope, const string& name, const string& type, int offset,
                         const vector<string>& allArgs, const string& retType, 
                        bool isOverride = false) {
        functions* newFunc = new functions(name, type, offset, allArgs, retType, isOverride);
        //for tests: cout<<"retType for "<< newFunc->name<<" is : "<<newFunc->ret_type<<endl; /////delete
        curScope->scope.push_back(newFunc);
    }

};

#endif // SYMBOL_TABLE_HPP