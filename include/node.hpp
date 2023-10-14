#pragma once

#include <vector>

using namespace std;

struct ops {
    static const char CONCAT = '.';
    static const char OR = '|';
    static const char STAR = '*';
    static const char LETTER = 'c';
};

struct terms {
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
    Node *getRightChild();
    void setRightChild(Node* node);
    void addChild(Node *child);
    void setParent(Node* parent);
    void replaceChild(Node* from, Node* to);
    void insertChildAfter(Node* left, Node* child);
    void removeChild(Node* child);
    char getOp();
    vector<Node*> getChildren();
    Node* getParent();
    Node* simplifyTree();

private:
    char op_;
    char letter_;
    vector<Node *> children_;
    Node* parent_ = nullptr;
};
