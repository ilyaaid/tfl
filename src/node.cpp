#include <iostream>
#include <algorithm>
#include "node.hpp"

using namespace std;

#include <algorithm>

Node::Node(char op) : op_(op)
{
}

Node::Node(char op, string letter) : op_(op), letter_(letter)
{
}

Node::~Node()
{
    for (int i = 0; i < children_.size(); ++i) {
        if (children_[i]) {
            delete children_[i];
        }
    }
}

Node *Node::getLeftChild(int i)
{
    if (i >= children_.size())
    {
        return nullptr;
    }
    return children_[i];
}

Node *Node::getRightChild(int i)
{
    int ind = children_.size() - (i + 1);
    if (ind < 0)
    {
        return nullptr;
    }
    return children_[ind];
}

void Node::setRightChild(Node *node)
{
    if (children_.size() >= 1)
    {
        children_[children_.size() - 1] = node;
    }
}

void Node::addChild(Node *child)
{
    children_.push_back(child);
}

void Node::setParent(Node *parent)
{
    parent_ = parent;
}

void Node::replaceChild(Node *from, Node *to)
{
    for (int i = 0; i < children_.size(); ++i)
    {
        if (from == children_[i])
        {
            children_[i] = to;
        }
    }
}


void Node::clearChildren()
{
    children_.clear();
}

char Node::getOp()
{
    return op_;
}

string Node::getLetter(){
    return letter_;
}

vector<Node *> Node::getChildren()
{
    return children_;
}

Node *Node::getParent()
{
    return parent_;
}

void Node::setOp(char op){
    op_ = op;
}

void Node::setLetter(string letter){
    letter_ = letter;
}

void Node::removeChild(Node *child)
{
    children_.erase(find(children_.begin(), children_.end(), child));
}

//--------------------------------------------------

string Node::getString()
{
    if (op_ == ops::LETTER)
    {
        string tmp_str = letter_;
        return tmp_str;
    }
    if (op_ == ops::STAR)
    {
        string tmp_str = children_[0]->getString();
        tmp_str.push_back('*');
        return tmp_str;
    }
    if (op_ == ops::CONCAT)
    {
        string tmp_str = "(";
        for (auto ch : children_)
        {
            tmp_str += ch->getString();
        }
        tmp_str += ")";
        return tmp_str;
    }
    if (op_ == ops::OR)
    {
        string tmp_str = "(";
        for (auto ch : children_)
        {
            tmp_str += ch->getString() + "|";
        }
        if (tmp_str.back() == '|')
        {
            tmp_str.pop_back();
        }
        tmp_str += ")";
        return tmp_str;
    }
    return "";
}

//-------------------simplify----------------------

Node *Node::simple()
{
    // избавляемся от ассоциативности, единственных детей в конкатенации или альтернативе
    Node *root = simplifyTree();

    // сильная звездная форма
    root = root->ssnf();
    root = root->simplifyTree();

    // избавляемся от коммутативности и идемпотентности
    root = root->aci();
    root = root->simplifyTree();

    // избавляемся от дистрибутивности
    root = root->dstr();
    root = root->simplifyTree();
    return root;
}

Node *Node::simplifyTree()
{
    if (op_ == ops::LETTER)
    {
        return this;
    }
    if (op_ == ops::STAR)
    {
        children_[0] = children_[0]->simplifyTree();
        return this;
    }

    // ops::CONCAT || ops::OR
    // здесь упрощение всех детей для конкатенации и или
    for (int i = 0; i < children_.size(); ++i)
    {
        children_[i] = children_[i]->simplifyTree();
    }
    //в этом цикле идет недопущение того, чтобы у конкатенации дети были конкатенация, а у или - или
    for (int i = 0; i < children_.size(); ++i)
    {
        Node *ch = children_[i];
        if (op_ == ch->getOp())
        {
            vector<Node *> ch_children = ch->getChildren();
            children_.insert(find(children_.begin(), children_.end(), ch), ch_children.begin(), ch_children.end());
            for (int i = 0; i < ch_children.size(); ++i)
            {
                ch_children[i]->setParent(this);
            }
            removeChild(ch);
        }
    }
    // здесь устранение конкатенации и или с одним ребенком
    if (children_.size() == 1)
    {
        Node *new_root = children_[0];
        if (parent_)
        {
            parent_->replaceChild(this, new_root);
        }
        new_root->setParent(parent_);
        children_.clear();
        delete this;
        return new_root;
    }
    return this;
}
