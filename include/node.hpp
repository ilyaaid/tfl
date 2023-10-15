#pragma once

#include <vector>
#include <string>

using namespace std;

struct ops
{
    static const char CONCAT = '.';
    static const char OR = '|';
    static const char STAR = '*';
    static const char LETTER = 'c';
};

struct terms
{
    static const char OR = '|';
    static const char STAR = '*';
    static const char L_BR = '(';
    static const char R_BR = ')';
};

class Node
{
public:
    Node(char op);
    Node(char op, char letter);

public:
    void setParent(Node *parent);
    Node *getParent();
    Node *getRightChild();
    void setRightChild(Node *node);
    void addChild(Node *child);
    void replaceChild(Node *from, Node *to);
    void removeChild(Node *child);
    void clearChildren();
    vector<Node *> getChildren();
    char getOp();

    //-------------------------

    string getString();

    // -------------------simplify

    Node *simple();

    Node *simplifyTree();

    bool checkEmptiness();

    Node *aci();
    Node* dstr();

    Node *ss();
    Node *ssnf();

private:
    char op_;
    char letter_;
    vector<Node *> children_;
    Node *parent_ = nullptr;
};
