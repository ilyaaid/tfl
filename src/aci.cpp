#include "node.hpp"

#include <map>
#include <set>

Node *Node::aci()
{
    if (op_ == ops::LETTER)
    {
        return this;
    }
    if (op_ == ops::STAR)
    {
        children_[0] = children_[0]->aci();
        return this;
    }
    if (op_ == ops::CONCAT)
    {
        for (int i = 0; i < children_.size(); ++i)
        {
            children_[i] = children_[i]->aci();
        }
        return this;
    }
    if (op_ == ops::OR)
    {
        map<string, Node *> mp_str_to_node;
        multiset<Node *> extra_nodes;
        for (int i = 0; i < children_.size(); ++i)
        {
            children_[i] = children_[i]->aci();
            mp_str_to_node[children_[i]->getString()] = children_[i];
            extra_nodes.insert(children_[i]);
        }
        children_.clear();
        for (auto pair : mp_str_to_node)
        {
            children_.push_back(pair.second);
            auto it = extra_nodes.find(pair.second);
            extra_nodes.erase(it);
        }

        // удаление из динамической памяти лишние аргументы альтернативы
        for (auto node : extra_nodes)
        {
            node->clearChildren();
            delete node;
        }
        return this;
    }
    return this;
}
