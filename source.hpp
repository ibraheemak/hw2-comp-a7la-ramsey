#ifndef HW3_TYPE_HPP
#define HW3_TYPE_HPP

#include <vector>
#include <string>

class TNode {
public:
    std::string type;
    TNode() = default;
    TNode(const std::string &type) : type(type) {}
    virtual ~TNode() = default;
};

class ID : public TNode {
public:
    std::string id;
    ID(const std::string &type, const std::string &id) : TNode(type), id(id) {}
};

class Num : public TNode {
public:
    int num;
    Num(const std::string &type, int val) : TNode(type), num(val) {}
};

class BoolLex : public TNode {
public:
    bool boolean;
    BoolLex(const std::string &type, bool boolean) : TNode(type), boolean(boolean) {}
};

class StringLex : public TNode {
public:
    std::string s;
    StringLex(const std::string &type, const std::string& s) : TNode(type), s(s) {}
};

class NamesLex : public TNode {
public:
    std::vector<std::string> names;
    NamesLex(const std::string &type, const std::vector<std::string>& names) : TNode(type), names(names) {}
};

class Binop : public TNode {
public:
    std::string op;
    Binop(const std::string &type, const std::string &op) : TNode(type), op(op) {}
};

class Relop : public TNode {
public:
    std::string op;
    Relop(const std::string &type, const std::string &op) : TNode(type), op(op) {}
};

#define YYSTYPE TNode*

#endif // HW3_TYPE_HPP