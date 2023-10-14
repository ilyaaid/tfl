#include <iostream>
#include <set>
#include "node.hpp"

using namespace std;

class GlushkovAutomat{
private:
    Node* _source_tree;


public:
    GlushkovAutomat (Node* source_tree);
    Node* getSourceTree();
    void linearizeTree();
    set create_lambda_set();

};