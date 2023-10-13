#include "parser.hpp"

#include <iostream>
#include <exception>

using namespace std;

void deleteExtraConc(Node* root) {
    vector<Node*> children = root->getChildren();
    if (root->getOp() == ops::CONCAT && children.size() == 1) {
        if (children[0]->getOp() == ops::CONCAT) {
            Node* child = children[0];
            Node* par_par = root->getParent();
            if (par_par) {
                par_par->replaceChild(root, child);
            }
            delete root;
        }
    }
    for (int i = 0; i < children.size(); ++i) {
        deleteExtraConc(children[i]);
    }
}

int main(int, char **)
{
    string regExpr = "";
    getline(cin, regExpr);
    RegExprParser parser;
    try
    {
        Node *root = parser.parse(regExpr, "abc");
        // deleteExtraConc(root);
        cout << "done!" << endl;
    }
    catch (const exception &err)
    {
        cout << err.what() << endl;
    }

    cout << endl;
    return 0;
}
