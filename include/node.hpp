#pragma once

#include <vector>
#include <string>
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
    void setParent(Node *parent);
    Node *getParent();
    Node *getLeftChild(int i);
    Node *getRightChild(int i);
    void setRightChild(Node *node);
    void addChild(Node *child);
    void replaceChild(Node *from, Node *to);
    void removeChild(Node *child);
    char getOp();
    void setOp(char op);
    string getLetter();
    void setLetter(string letter);
    vector<Node*> getChildren();
    void clearChildren();

    Node *simplifyTree();

    bool checkEmptiness();

    Node *aci();
    Node *dstr();

    Node *ss();
    Node *ssnf();

    Node* simple();

    string getString();


private:
    char op_;
    string letter_;
    vector<Node *> children_;
    Node *parent_ = nullptr;
};
