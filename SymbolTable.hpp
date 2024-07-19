#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <vector>
#include <string>
#include "hw3_output.hpp"

using namespace std;


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

    size_t size() const {
        return offset_Stack.size();
    }
};


////////////////////////////////////////

/// creating a table entry that will save local variables - t6
class tableEntry {
public:
    string name;
    string type;
    int offset;
    tableEntry(const string& name, const string& type, int offset)
        : name(name), type(type), offset(offset) {}
};

// scope entry for every new scope 
class ScopeBlock {
public:
    vector<tableEntry*> scope;
    ScopeBlock* parent;

    ScopeBlock() : parent(nullptr) {}
    ScopeBlock(ScopeBlock* parent) : parent(parent) {}
};


// !! check this
class functions : public tableEntry {
public:
    int numofarg;
    vector<string> all_arg;
    string ret_type;
    bool isOverride;

    functions(string the_name, string the_type, int the_offset, int the_num,
              vector<string> all_the_arg, string the_ret_type, bool the_isOverride)
        : tableEntry(the_name, the_type, the_offset), numofarg(the_num), all_arg(all_the_arg),
          ret_type(the_ret_type), isOverride(the_isOverride) {}
};

class TablesStack {
public:
    vector<ScopeBlock*> stackTable;
    ScopeBlock* ParentScope;

    TablesStack() {
        ParentScope = InsertTable(nullptr);
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
                        int numArgs, const vector<string>& allArgs, const string& retType, 
                        bool isOverride = false) {
        functions* newFunc = new functions(name, type, offset, numArgs, allArgs, retType, isOverride);
        curScope->scope.push_back(newFunc);
    }

};

#endif // SYMBOL_TABLE_HPP