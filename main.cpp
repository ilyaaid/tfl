#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

#include "grammar.hpp"

using namespace std;

bool isProd = false;

set<string> split_to_set(const string &s)
{
    set<string> st;
    std::string token;
    std::stringstream sstream(s);
    while (getline(sstream, token, ' '))
    {
        st.insert(token);
    }
    return st;
}

Grammar input_grammar()
{
    cin.ignore();

    // if (isProd)
    // {
    //     cout << "Введите терминалы(через пробел):" << endl;
    // }
    // string terms;
    // getline(cin, terms);

    // if (isProd)
    // {
    //     cout << "Введите нетерминалы(через пробел):" << endl;
    // }
    // string nterms;
    // getline(cin, nterms);

    if (isProd) {
        cout << "Введите n-ый шаг разбора, на котором надо отрисовать стек(0 - если отрисовывать стек не надо)" << endl;
    }
    int n = 0;
    cin >> n;

    if (isProd)
    {
        cout << "В грамматике нетерминалы - [A-Z], остальные символы - терминалы(# - пустой символ)" << endl;
        cout << "Введите стартовый нетерминал:" << endl;
    }
    char start;
    cin >> start;

    if (isProd)
    {
        cout << "Введите правила грамматики\n"
                "1) правило вида: A=w (пробелов нет нигде)\n"
                "2) каждое правило на новой строке\n"
                "3) окончание ввода - Ctrl+D:"
             << endl;
    }

    vector<string> rules;
    string rule;
    while (cin >> rule)
    {
        rules.push_back(rule);
    }

    if (n == 0) { 
        return Grammar(start, rules);
    } else {
        return Grammar(start, rules, n);
    }
}


/*

Примеры ввода:
1)
acdccdd
2
A
A=BDc
A=a
B=b
B=#
D=d
D=#

2)
dc
A
A=BDc
A=a
B=b
B=#
D=d

3)
(1+1)
S
S=F
S=(S+F)
F=1

4)
n+n*(n+n)
E
E=TA
A=+TA
A=#
T=FB
B=*FB
B=#
F=n
F=(E)

5)
ab
2
S
S=aSb
S=#
S=ac

6)
(()a)
1
S
S=(L)
S=a
L=SA
A=)SA
A=#

7)
((a)a))
1
S
S=(L)
S=a
L=SA
A=)SA
A=#


*/

int main(int, char **)
{
    if (isProd)
    {
        cout << "Введите входное слово:" << endl;
    }
    string input_word;
    cin >> input_word;

    try
    {
        Grammar g = input_grammar();

        g.buildFirst();
        g.buildFollow();
        g.buildTable();

        string message;
        bool is_ok = g.checkWord(input_word, message);
        if (is_ok) {
            cout << "word is in grammar!" << endl;
        } else {
            cout << "--- error in 'check word': " << message << endl;
        }
    }
    catch (runtime_error e)
    {
        cout << e.what() << endl;
    }

    cout << endl;

    return 0;
}
