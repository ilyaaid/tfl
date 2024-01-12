#include "grammar.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

Grammar::Grammar(char start,
                 const vector<string> &rules, int action_for_painting)
    : _start(start), _action_for_painting(action_for_painting)
{
    if (isTerm(start))
    {
        throw runtime_error("Стартовый символ должен быть [A-Z]");
    }
    parseRules(rules);
}

bool Grammar::isTerm(char el)
{
    return !(el >= 'A' && el <= 'Z');
}

void Grammar::parseRules(vector<string> rules)
{
    for (int i = 0; i < rules.size(); ++i)
    {
        string rule = rules[i];
        int sep_ind = rule.find('=');
        char left = rule[0];
        if (isTerm(left) || rule[1] != '=')
        {
            throw runtime_error("Неверный формат правила");
        }

        string right(rule.begin() + 2, rule.end());
        _nterms.insert(left);
        for (int i = 0; i < right.size(); ++i)
        {
            if (isTerm(right[i]))
            {
                _terms.insert(right[i]);
            }
        }
        _terms.insert(end);
        _terms.erase(eps);
        _rules.push_back({left, right});
        _add_first_for_rule.push_back({});
        _nt_to_rules[left].push_back(i);
    }
}

//======================== построение множества FIRST

void Grammar::buildFirst()
{
    firstForNTerm();
}

void Grammar::firstForNTerm()
{
    bool changed = true;

    while (changed)
    {
        changed = false;
        for (auto nt : _nterms)
        {
            vector<int> rules = _nt_to_rules[nt];
            for (int i = 0; i < rules.size(); ++i)
            {
                int x = _first[nt].size();

                set<char> dop_set = firstForNTermRule(_rules[rules[i]].to);
                _add_first_for_rule[rules[i]] = dop_set;
                _first[nt].insert(dop_set.begin(), dop_set.end());

                if (_first[nt].size() != x)
                {
                    changed = true;
                }
            }
        }
    }
}

set<char> Grammar::firstForNTermRule(string to)
{
    // string to = r.to;
    if (isTerm(to[0]))
    {
        if (to.size() > 1 && to[0] == eps)
        {
            throw runtime_error("Ничего не должно быть рядом с пустым символом");
        }
        return {to[0]};
    }
    else
    {
        set<char> next_first = _first[to[0]];
        // здесь проверяем, случай перехода из нетерминала в один нетерминал
        // или наличие пустого символа в множестве следующего нетерминала
        if (to.size() == 1 || next_first.find(eps) == next_first.end())
        {
            return next_first;
        }
        else
        {
            next_first.erase(eps);
            to.erase(to.begin());
            set<char> next_next_first = firstForNTermRule(to);
            next_first.insert(next_next_first.begin(), next_next_first.end());
            return next_first;
        }
    }
}

//=============== построение множества FOLLOW

void Grammar::buildFollow()
{
    _follow[_start] = {end};
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < _rules.size(); ++i)
        {
            char from = _rules[i].from;
            string to = _rules[i].to;
            for (int j = 0; j < to.size(); ++j)
            {
                if (!isTerm(to[j]))
                {
                    int x = _follow[to[j]].size();
                    if (j == to.size() - 1)
                    {
                        _follow[to[j]].insert(_follow[from].begin(), _follow[from].end());
                    }
                    else
                    {
                        set<char> next_first = firstForNTermRule(string(to.begin() + j + 1, to.end()));
                        bool is_eps = next_first.find(eps) != next_first.end();
                        next_first.erase(eps);
                        _follow[to[j]].insert(next_first.begin(), next_first.end());
                        if (is_eps)
                        {
                            _follow[to[j]].insert(_follow[from].begin(), _follow[from].end());
                        }
                    }
                    if (_follow[to[j]].size() != x)
                    {
                        changed = true;
                    }
                }
            }
        }
    }
}

//================ Построение таблицы
void Grammar::buildTable()
{
    for (auto nt : _nterms)
    {
        _table[nt] = {};
        for (auto t : _terms)
        {
            _table[nt][t] = {};
            vector<int> nt_rules = _nt_to_rules[nt];
            for (int i = 0; i < nt_rules.size(); ++i)
            {
                int rule_ind = nt_rules[i];
                if (_add_first_for_rule[rule_ind].find(t) != _add_first_for_rule[rule_ind].end() ||
                    _add_first_for_rule[rule_ind].find(eps) != _add_first_for_rule[rule_ind].end() &&
                        _follow[nt].find(t) != _follow[nt].end())
                {
                    _table[nt][t].push_back(rule_ind);
                }
            }
        }
    }
}

//================ Проверка слова
bool Grammar::checkWord(string word, string &message)
{
    word += end;
    vector<Node *> nodes;

    Node *beg = new Node(_start);
    beg->parent = new Node(end);
    nodes.push_back(beg);
    nodes.push_back(beg->parent);

    set<pair<Node *, int>> cur_nodes; // указатель на вершину с элементом и позиция на ленте
    cur_nodes.insert({beg, 0});

    int error_ind = 0;
    bool is_ok = false;
    while (!cur_nodes.empty())
    {
        set<pair<Node *, int>> new_cur_nodes;
        bool is_final = false;
        for (auto p_cur_node : cur_nodes)
        {
            char lenta_elem = word[p_cur_node.second];
            Node *cur_node = p_cur_node.first;

            if (isTerm(cur_node->s))
            {
                if (lenta_elem == end && cur_node->s == end)
                {
                    is_final = true;
                    is_ok = true;
                    break;
                }
                if (lenta_elem == cur_node->s)
                {
                    int new_lenta_ind = p_cur_node.second + 1;
                    new_cur_nodes.insert({cur_node->parent, new_lenta_ind});
                    error_ind = max(error_ind, new_lenta_ind); // маскимальный достижимый индекс
                }
            }
            else
            {
                // удаляем нетерминал и добавляем развертку нетерминала
                vector<int> rules_ind = _table[cur_node->s][lenta_elem];
                for (int i = 0; i < rules_ind.size(); ++i)
                {
                    Rule r = _rules[rules_ind[i]];
                    Node *child = cur_node->parent;
                    for (int j = r.to.size() - 1; j >= 0; j--)
                    {
                        if (r.to[j] != eps)
                        {
                            Node *new_node = new Node(r.to[j]);
                            nodes.push_back(new_node);
                            new_node->parent = child;
                            child = new_node;
                        }
                    }
                    new_cur_nodes.insert({child, p_cur_node.second});

                    set<Node*> stack_for_print;
                    for (auto p : new_cur_nodes) {
                        stack_for_print.insert(p.first);
                    }
                    tryPrintingStack(stack_for_print);
                }
            }
        }
        if (is_final)
        {
            break;
        }
        cur_nodes = new_cur_nodes;
    }

    for (int i = 0; i < nodes.size(); ++i)
    {
        delete nodes[i];
    }

    if (!is_ok)
    {
        message = "error in " + to_string(error_ind); // выводим самый дальний индекс, до которого дошли
    }

    return is_ok;
}

void Grammar::tryPrintingStack(set<Node*> nodes)
{
    if (_action_for_painting < 0)
    {
        return;
    }
    _cnt_actions++;
    if (_cnt_actions == _action_for_painting) {
        printStack(nodes);
    }
}

void Grammar::printStack(set<Node*> nodes)
{
    stringstream ss;
    ss << "digraph G { " << endl;

    map<Node*, int> node_to_int;
    int mx_vex_num = 1;

    while(!nodes.empty()) {
        set<Node*> new_nodes;
        for (auto node : nodes) {
            if (node_to_int.find(node) == node_to_int.end()) {
                node_to_int[node] = mx_vex_num;
                mx_vex_num++;
            }
            if (node_to_int.find(node->parent) == node_to_int.end()) {
                node_to_int[node->parent] = mx_vex_num;
                mx_vex_num++;
            }

            ss << "\t" << node_to_int[node] << " [label = \"" << node->s << "\"];" << endl;
            if (node->parent) {
                ss << "\t" << node_to_int[node] << " -> " << node_to_int[node->parent] << ";" << endl;
                new_nodes.insert(node->parent);
            }
        }
        nodes = new_nodes;
    }

    ss << "}" << endl;

    ofstream outFile;
    outFile.open("stack.dot");
    outFile << ss.rdbuf();
    outFile.close();
}
