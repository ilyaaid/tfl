#include "parser.hpp"

#include <iostream>
#include <algorithm>
#include <exception>
#include <sstream>

using namespace std;

bool RegExprParser::isInAlpabet(char c)
{
    return alphabet_.find(c) < alphabet_.size();
}

bool RegExprParser::isSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\r' ||
            c == '\t' || c == '\v' || c == '\f');
}

void RegExprParser::initParams(const string &regexpr, const string &alphabet)
{
    alphabet_ = alphabet;
    regexpr_ = regexpr;
    // удаляем пробелы
    regexpr_.erase(std::remove_if(regexpr_.begin(), regexpr_.end(), isSpace), regexpr_.end());
}

Node *RegExprParser::parse(const string &regexpr, const string &alphabet)
{
    initParams(regexpr, alphabet);

    Node *root = new Node(ops::CONCAT);

    for (int i = 0; i < regexpr_.size(); ++i)
    {
        if (isInAlpabet(regexpr_[i]))
        {
            string str;
            str.push_back(regexpr_[i]);
            Node *node = new Node(ops::LETTER, str);
            root->addChild(node);
            node->setParent(root);
            continue;
        }
        switch (regexpr_[i])
        {
        case terms::L_BR:
        {
            Node *new_root = new Node(ops::CONCAT);
            root->addChild(new_root);
            new_root->setParent(root);
            root = new_root;
            break;
        }
        case terms::STAR:
        {
            Node *new_star = new Node(ops::STAR);
            new_star->addChild(root->getRightChild());
            root->getRightChild()->setParent(new_star);
            root->setRightChild(new_star);
            new_star->setParent(root);
            break;
        }
        case terms::OR:
        {
            Node *new_or = new Node(ops::OR);
            new_or->addChild(root);
            Node *new_conc = new Node(ops::CONCAT);
            new_or->addChild(new_conc);

            Node *parent = root->getParent();
            if (parent)
            {
                parent->setRightChild(new_or);
                new_or->setParent(parent);
            }

            root->setParent(new_or);
            new_conc->setParent(new_or);

            root = new_conc;
            break;
        }
        case terms::R_BR:
        {
            Node* root_par = root->getParent();
            if (!root_par) {
                throw runtime_error("wrong symbol " + terms::R_BR);
            }
            root = root_par;
            if (root->getOp() == ops::CONCAT) {
                break;
            }
            
            Node* new_conc = new Node(ops::CONCAT);
            new_conc->addChild(root);
            root_par = root->getParent();
            root->setParent(new_conc);

            if (root_par) {
                root_par->setRightChild(new_conc);
                new_conc->setParent(root_par);
            }
            root = new_conc;
            break;
        }
        }
    }

    while (root->getParent())
    {
        root = root->getParent();
    }
    return root;
}
