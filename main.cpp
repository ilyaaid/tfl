#include "parser.hpp"

#include <iostream>
#include <exception>

using namespace std;

int main(int, char **)
{
    string regExpr = "";
    getline(cin, regExpr);
    RegExprParser parser;
    try
    {
        Node *root = parser.parse(regExpr, "abcde");
        root = root->simplifyTree();
        root->ssnf();
        cout << "done!" << endl;
    }
    catch (const exception &err)
    {
        cout << err.what() << endl;
    }

    cout << endl;
    return 0;
}
