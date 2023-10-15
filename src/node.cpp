#include "node.hpp"

#include <algorithm>

Node::Node(char op) : op_(op)
{
}

Node::Node(char op, char letter) : op_(op), letter_(letter)
{
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

void Node::removeChild(Node *child)
{
    children_.erase(find(children_.begin(), children_.end(), child));
}

void Node::clearChildren()
{
    children_.clear();
}

char Node::getOp()
{
    return op_;
}

vector<Node *> Node::getChildren()
{
    return children_;
}

Node *Node::getParent()
{
    return parent_;
}

//--------------------------------------------------

string Node::getString()
{
    if (op_ == ops::LETTER)
    {
        string tmp_str = "";
        tmp_str.push_back(letter_);
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
    Node *root = simplifyTree();
    root = root->ssnf();
    root = root->simplifyTree();
    root = root->aci();
    root = root->simplifyTree();
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
    for (int i = 0; i < children_.size(); ++i)
    {
        children_[i] = children_[i]->simplifyTree();
    }
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
