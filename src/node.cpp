#include "node.hpp"

#include <algorithm>

Node::Node(char op) : op_(op)
{
}

Node::Node(char op, char letter) : op_(op), letter_(letter)
{
}

Node *Node::getRightChild()
{
    if (children_.size() >= 1)
    {
        return children_[children_.size() - 1];
    }
    return nullptr;
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

bool Node::checkEmptiness()
{
    if (op_ == ops::LETTER)
    {
        return false;
    }
    if (op_ == ops::STAR)
    {
        return true;
    }
    if (op_ == ops::CONCAT)
    {
        for (auto child : children_)
        {
            if (!child->checkEmptiness())
            {
                return false;
            }
        }
        return true;
    }
    if (op_ == ops::OR)
    {
        for (auto child : children_)
        {
            if (child->checkEmptiness())
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

Node *Node::ssnf()
{
    if (op_ == ops::LETTER)
    {
        return this;
    }
    if (op_ == ops::CONCAT || op_ == ops::OR)
    {
        for (auto ch : children_)
        {
            ch = ch->ssnf();
        }
        return this;
    }
    if (op_ == ops::STAR)
    {
        children_[0] = children_[0]->ss();
        return this;
    }
    return this;
}

Node *Node::ss()
{
    if (op_ == ops::LETTER)
    {
        return this;
    }
    if (op_ == ops::OR)
    {
        for (auto ch : children_)
        {
            ch = ch->ss();
        }
        return this;
    }
    if (op_ == ops::CONCAT)
    {
        if (checkEmptiness())
        {
            Node *new_or = new Node(ops::OR);
            for (auto ch : children_)
            {
                ch = ch->ss();
                new_or->addChild(ch);
                ch->setParent(new_or);
            }
            if (parent_)
            {
                parent_->replaceChild(this, new_or);
            }
            new_or->setParent(parent_);
            children_.clear();
            delete this;
            return new_or;
        }
        for (auto ch : children_)
        {
            ch = ch->ssnf();
        }

        return this;
    }
    if (op_ == ops::STAR)
    {
        children_[0]->ss();
        Node* new_root = children_[0];
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
