#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <queue>
#include "glushkov.hpp"

using namespace std;

set<string> multiply_to_lambda(set<string> set1, bool lambda)
{
    set<string> empty_set;
    if (lambda)
    {
        return set1;
    }
    else
    {
        return empty_set;
    }
}

set<pair<string, string>> pair_multiply(set<string> set1, set<string> set2)
{
    set<pair<string, string>> res;
    for (auto elem1 : set1)
    {
        for (auto elem2 : set2)
        {
            res.insert({elem1, elem2});
        }
    }
    return res;
}

set<string> sets_union(set<string> set1, set<string> set2)
{
    for (auto elem : set1)
    {
        set2.insert(elem);
    }
    return set2;
}

set<pair<string, string>> pair_sets_union(set<pair<string, string>> set1, set<pair<string, string>> set2)
{
    for (auto elem : set1)
    {
        set2.insert(elem);
    }
    return set2;
}

set<string> sets_intersection(set<string> set1, set<string> set2)
{
    set<string> res;
    set_intersection(begin(set1), end(set1), begin(set2), end(set2), inserter(res, begin(res)));
    return res;
}

int linearize_tree(Node *root, int count)
{
    if (root != nullptr)
    {
        if (root->getOp() == ops::LETTER)
        {
            root->setLetter(root->getLetter() + to_string(count));
            count++;
        }
        for (auto elem : root->getChildren())
        {
            count = linearize_tree(elem, count);
        }
    }
    return count;
}

GlushkovAutomat::GlushkovAutomat(Node *source_tree) : _source_tree(source_tree) {}

void GlushkovAutomat::init()
{
    linearizeTree();
    findPSet();
    findDSet();
    findFSet();
    make_automat();
}

Node *GlushkovAutomat::getSourceTree()
{
    return _source_tree;
}

void GlushkovAutomat::linearizeTree()
{
    linearize_tree(_source_tree, 1);
}

bool find_lambda(Node *node)
{
    if (node->getOp() == ops::CONCAT)
    {
        for (auto elem : node->getChildren())
        {
            if (!find_lambda(elem))
            {
                return false;
            }
        }
        return true;
    }
    if (node->getOp() == ops::OR)
    {
        for (auto elem : node->getChildren())
        {
            if (find_lambda(elem))
            {
                return true;
            }
        }
        return false;
    }
    if (node->getOp() == ops::STAR)
    {
        return true;
    }
    if (node->getOp() == ops::LETTER)
    {
        return false;
    }
    return false;
}

set<string> find_p_set(Node *parent, vector<Node *> children)
{
    if (parent->getOp() == ops::OR)
    {
        Node *last_operand = children.back();
        if (children.size() == 1)
        {
            return find_p_set(last_operand, last_operand->getChildren());
        }
        children.pop_back();
        return sets_union(find_p_set(parent, children),
                          find_p_set(last_operand, last_operand->getChildren()));
    }
    if (parent->getOp() == ops::CONCAT)
    {
        Node *last_operand = children.back();
        if (children.size() == 1)
        {
            return find_p_set(last_operand, last_operand->getChildren());
        }
        children.pop_back();
        return sets_union(find_p_set(parent, children),
                          multiply_to_lambda(find_p_set(last_operand, last_operand->getChildren()),
                                             find_lambda(parent)));
    }
    if (parent->getOp() == ops::STAR)
    {
        Node *last_operand = children.back();
        return find_p_set(last_operand, last_operand->getChildren());
    }
    if (parent->getOp() == ops::LETTER)
    {
        string str = parent->getLetter();
        set<string> res{str};
        return res;
    }
    return set<string>{};
}

void GlushkovAutomat::findPSet()
{
    _p_set = find_p_set(_source_tree, _source_tree->getChildren());
}

set<string> GlushkovAutomat::getPSet()
{
    return _p_set;
}

set<string> find_d_set(Node *parent, vector<Node *> children)
{
    if (parent->getOp() == ops::OR)
    {
        Node *last_operand = children.back();
        if (children.size() == 1)
        {
            return find_d_set(last_operand, last_operand->getChildren());
        }
        children.pop_back();
        return sets_union(find_d_set(parent, children),
                          find_d_set(last_operand, last_operand->getChildren()));
    }
    if (parent->getOp() == ops::CONCAT)
    {
        Node *last_operand = children.back();
        if (children.size() == 1)
        {
            return find_d_set(last_operand, last_operand->getChildren());
        }
        children.pop_back();
        return sets_union(find_d_set(last_operand, last_operand->getChildren()),
                          multiply_to_lambda(find_d_set(parent, children), find_lambda(last_operand)));
    }
    if (parent->getOp() == ops::STAR)
    {
        Node *last_operand = children.back();
        return find_d_set(last_operand, last_operand->getChildren());
    }
    if (parent->getOp() == ops::LETTER)
    {
        string str = parent->getLetter();
        set<string> res{str};
        return res;
    }
    return set<string>{};
}

void GlushkovAutomat::findDSet()
{
    _d_set = find_d_set(_source_tree, _source_tree->getChildren());
}

set<string> GlushkovAutomat::getDSet()
{
    return _d_set;
}

set<pair<string, string>> find_f_set(Node *parent, vector<Node *> children)
{
    if (parent->getOp() == ops::OR)
    {
        Node *last_operand = children.back();
        if (children.size() == 1)
        {
            return find_f_set(last_operand, last_operand->getChildren());
        }
        children.pop_back();
        return pair_sets_union(find_f_set(parent, children),
                               find_f_set(last_operand, last_operand->getChildren()));
    }
    if (parent->getOp() == ops::CONCAT)
    {
        Node *last_operand = children.back();
        if (children.size() == 1)
        {
            return find_f_set(last_operand, last_operand->getChildren());
        }
        children.pop_back();
        return pair_sets_union(find_f_set(parent, children),
                               pair_sets_union(find_f_set(last_operand, last_operand->getChildren()),
                                               pair_multiply(find_d_set(parent, children),
                                                             find_p_set(last_operand, last_operand->getChildren()))));
    }
    if (parent->getOp() == ops::STAR)
    {
        Node *last_operand = children.back();
        return pair_sets_union(find_f_set(last_operand, last_operand->getChildren()),
                               pair_multiply(find_d_set(last_operand, last_operand->getChildren()),
                                             find_p_set(last_operand, last_operand->getChildren())));
    }
    if (parent->getOp() == ops::LETTER)
    {
        return set<pair<string, string>>{};
    }
    return set<pair<string, string>>{};
}

void GlushkovAutomat::findFSet()
{
    _f_set = find_f_set(_source_tree, _source_tree->getChildren());
}

set<pair<string, string>> GlushkovAutomat::getFSet()
{
    return _f_set;
}

void GlushkovAutomat::make_automat()
{
    for (auto elem : _p_set)
    {
        automat["start_elem"].push_back(elem);
    }
    for (auto elem : _f_set)
    {
        automat[elem.first].push_back(elem.second);
    }
}

int Path::count_len_path()
{
    return vec.size() - cicle_indexes.size() + cicle_indexes.size() * pump_len;
}

string Path::make_string()
{
    string res = "";
    for (int i = 0; i < vec.size(); i++)
    {
        if (ciclic == true &&
            find(cicle_indexes.begin(), cicle_indexes.end(), i) != cicle_indexes.end())
        {
            for (int j = 0; j < pump_len; j++)
            {
                res += vec[i][0];
            }
        }
        else
        {
            res += vec[i][0];
        }
    }
    return res;
}

vector<Path> GlushkovAutomat::create_words()
{
    int max_cnt_words = 200;
    vector<Path> words;
    queue<Path> q;
    for (auto elem : automat["start_elem"])
    {
        Path a;
        a.vec.push_back(elem);
        if (count(automat[elem].begin(), automat[elem].end(), elem) != 0)
        {
            a.ciclic = true;
            a.cicle_indexes.push_back(a.vec.size() - 1);
        }
        q.push(a);
    }
    while (!q.empty())
    {
        Path front = q.front();
        if (find(_d_set.begin(), _d_set.end(), front.vec.back()) != _d_set.end())
        {
            words.push_back(front);
            if (words.size() >= max_cnt_words)
            {
                break;
            }
        }
        q.pop();
        for (auto elem : automat[front.vec.back()])
        {
            // здесь ошибка
            Path x = front;
            x.vec.push_back(elem);
            if (count(automat[elem].begin(), automat[elem].end(), elem) != 0)
            {
                x.ciclic = true;
                x.cicle_indexes.push_back(x.vec.size() - 1);
            }
            if (x.count_len_path() <= x.max_str_len)
            {
                q.push(x);
            }
            // front.vec.pop_back(); // new
        }
    }
    return words;
}
