#include "node.hpp"

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
