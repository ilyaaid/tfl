#include "node.hpp"

Node::Node(char op) : op_(op)
{
}

Node::Node(char op, char letter): op_(op), letter_(letter)
{
}

Node *Node::getRightChild()
{
    if (children_.size() >= 1) {
        return children_[children_.size() - 1];
    }
    return nullptr;
}

void Node::setRightChild(Node* node)
{
    if (children_.size() >= 1) {
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
    for (int i = 0; i < children_.size(); ++i) {
        if (from == children_[i]) {
            children_[i] = to;
        }
    }
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
