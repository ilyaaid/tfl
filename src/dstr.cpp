#include "node.hpp"
#include <set>

Node *Node::dstr()
{
    // на входе всегда aci-нормализованное выражение!
    if (op_ == ops::STAR)
    {
        children_[0] = children_[0]->dstr();
        return this;
    }
    if (op_ == ops::LETTER)
    {
        return this;
    }
    if (op_ == ops::CONCAT)
    {
        for (int i = 0; i < children_.size(); ++i)
        {
            children_[i] = children_[i]->dstr();
        }
        return this;
    }
    if (op_ == ops::OR)
    {
        int cnt_concat = 0;
        for (int i = 0; i < children_.size(); ++i)
        {
            children_[i] = children_[i]->dstr();
            if (children_[i]->getOp() == ops::CONCAT)
            {
                cnt_concat++;
            }
        }

        if (cnt_concat == children_.size())
        {
            // получает первый левый индекс несовпдающих элементов
            int first_pref = 0;
            while (true)
            {
                string first_regexp = children_[0]->getLeftChild(first_pref)->getString();
                bool is_break = false;
                for (int i = 1; i < children_.size(); ++i)
                {
                    if (children_[i]->getLeftChild(first_pref)->getString() != first_regexp)
                    {
                        is_break = true;
                        break;
                    }
                }
                if (is_break)
                {
                    break;
                }
                first_pref++;
            }
            // получает первый правый индекс несовпдающих элементов
            int first_suf = 0;
            while (true)
            {
                string first_regexp = children_[0]->getRightChild(first_suf)->getString();
                bool is_break = false;
                for (int i = 1; i < children_.size(); ++i)
                {
                    if (children_[i]->getRightChild(first_suf)->getString() != first_regexp)
                    {
                        is_break = true;
                        break;
                    }
                }
                if (is_break)
                {
                    break;
                }
                first_suf++;
            }
            // затем создаем новые узлы(вынос за скобки), если first_pref и first_suf ненулевые
            if (first_pref != 0 || first_suf != 0)
            {
                Node *par_conc = new Node(ops::CONCAT);
                if (parent_)
                {
                    parent_->replaceChild(this, par_conc);
                }
                par_conc->setParent(parent_);

                // добавляем левого ребенка(умножаем общий префикс)
                if (first_pref != 0)
                {
                    Node *left_conc = new Node(ops::CONCAT);
                    par_conc->addChild(left_conc);
                    left_conc->setParent(par_conc);

                    for (int i = 0; i < first_pref; ++i)
                    {
                        Node *left_child = children_[0]->getLeftChild(i);
                        left_conc->addChild(left_child);
                        left_child->setParent(left_conc);

                        for (int j = 0; j < children_.size(); ++j)
                        {
                            children_[j]->removeChild(children_[j]->getLeftChild(i));
                        }
                    }
                }
                // добавляем альтернативу(основную)
                par_conc->addChild(this);
                // добавляем правого ребенка(умножаем общий суффикс)
                if (first_suf != 0)
                {
                    Node *right_conc = new Node(ops::CONCAT);
                    par_conc->addChild(right_conc);
                    right_conc->setParent(par_conc);

                    for (int i = 0; i < first_suf; ++i)
                    {
                        Node *right_child = children_[0]->getRightChild(i);
                        right_conc->addChild(right_child);
                        right_child->setParent(right_conc);

                        for (int j = 0; j < children_.size(); ++j)
                        {
                            children_[j]->removeChild(children_[j]->getRightChild(i));
                        }
                    }
                }
                return par_conc;
            }
        }
        return this;
    }
    return this;
}
