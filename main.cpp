#include "parser.hpp"
#include "glushkov.hpp"

#include <iostream>
#include <exception>
#include <set>
#include <algorithm>

using namespace std;


int main(int, char **)
{   
    string regExpr = "";
    getline(cin, regExpr);
    RegExprParser parser;
    try
    {
        Node *root = parser.parse(regExpr, "abcde");
        GlushkovAutomat automat(root);
        automat.linearizeTree();
        automat.findPSet();
        automat.findDSet();
        automat.findFSet();
        set<string> set2 = automat.getPSet();
        set<string> set3 = automat.getDSet();
        set<pair_of_vertices, Cmp> set4 = automat.getFSet();
        root = automat.getSourceTree();       
        cout << "done!" << endl;
        // root = root->simplifyTree();
        // root->ssnf();
    }
    catch (const exception &err)
    {
        cout << err.what() << endl;
    }
    cout << endl;
    return 0;
}
