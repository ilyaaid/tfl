#pragma once

#include <vector>
#include <iostream>

using namespace std;

struct ops
{
    static const char CONCAT = '.';
    static const char OR = '|';
    static const char STAR = '*';
    static const char LETTER = 'c';
    static const char EPS = '?';
    static const char EMPTY = ' ';
    
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
    Node(char op, string letter);

public:
    Node *getRightChild();
    void setRightChild(Node *node);
    void addChild(Node *child);
    void setParent(Node *parent);
    void replaceChild(Node *from, Node *to);
    void removeChild(Node *child);
    char getOp();
    void setOp(char op);
    string getLetter();
    void setLetter(string letter);
    
    void removeChild(Node* child);
    vector<Node*> getChildren();
    Node* getParent();

    Node* simplifyTree();
    
    void ss();
    void ssnf();


private:
    char op_;
    string letter_;
    vector<Node *> children_;
    Node *parent_ = nullptr;
};
