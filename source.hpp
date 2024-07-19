#ifndef HW3_TYPE_HPP
#define HW3_TYPE_HPP

#include <vector>
#include <string>
using namespace std;

class TNode {
public:
    string type;
    TNode() = default;
    TNode(const string &type) : type(type) {}
    virtual ~TNode() = default;
};

class IdentifierStr : public TNode {
public:
    string id;
    IdentifierStr(const string &type, string id) : TNode(type), id(id) {}
};

class Num : public TNode {
public:
    int num;
    Num(const string &type, int val) : TNode(type), num(val) {}
};

class BoolLex : public TNode {
public:
    bool boolean;
    BoolLex(const string &type, bool boolean) : TNode(type), boolean(boolean) {}
};

class StringLex : public TNode {
public:
    string s;
    StringLex(const string &type, const string& s) : TNode(type), s(s) {}
};

class Binop : public TNode {
public:
    string op;
    Binop(const string &type, const string &op) : TNode(type), op(op) {}
};

class Relop : public TNode {
public:
    string op;
    Relop(const string &type, const string &op) : TNode(type), op(op) {}
};

class ExpNode : public TNode {
public:
    ExpNode(const string &type) : TNode(type) {}
};

class StmtNode : public TNode {
public:
    StmtNode(const string &type) : TNode(type) {}
};
#define YYSTYPE TNode*
#endif // HW3_TYPE_HPP