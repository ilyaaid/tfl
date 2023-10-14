#include "parser.hpp"
#include "glushkov.hpp"

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
        Node *root = parser.parse(regExpr, "abc");
        root = root -> simplifyTree();
        GlushkovAutomat automat(root);
        automat.linearizeTree();
        root = automat.getSourceTree();
        cout << "done!" << endl;
    }
    catch (const exception &err)
    {
        cout << err.what() << endl;
    }
    cout << endl;
    return 0;
}
