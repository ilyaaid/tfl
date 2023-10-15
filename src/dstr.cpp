#include "node.hpp"

Node *Node::dstr()
{
    if (op_ == ops::STAR) {
        children_[0] = children_[0]->dstr();
        return children_[0];
    }
    if (op_ == ops::LETTER) {
        return this;
    }
    if (op_ == ops::CONCAT) {
        
    }
    return nullptr;
}

