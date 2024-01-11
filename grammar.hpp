#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>

using namespace std;

struct Rule {
    char from = ' ';
    string to;
};

struct Node {
    char s;
    Node* child = nullptr;

    Node(char sym): s(sym){};
};

struct NodeCur {
    Node* n = nullptr;
    int s_ind = 0;
};

class Grammar
{
public:
    Grammar(char start,
            const vector<string> &rules);

    void buildFirst();
    void buildFollow();
    void buildTable();
    bool checkWord(string word, string& message);

private:
    void firstForNTerm();
    set<char> firstForNTermRule(string to);

    void parseRules(vector<string> rules);
    bool isTerm(char el);
private:
    const char eps = '#';
    const char end = '$';

    char _start;
    set<char> _terms;
    set<char> _nterms;

    //таблица синтаксического разбора
    //1 параметр - нетерминал
    //2 параметр - терминал
    map<char, map<char, vector<int> >> _table;

    vector<Rule> _rules;
    vector<set<char>> _add_first_for_rule;

    map<char, vector<int>> _nt_to_rules;
    map<char, set<char>> _first;
    map<char, set<char>> _follow;
};
