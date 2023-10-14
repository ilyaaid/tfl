#include <iostream>
#include <set>
#include "glushkov.hpp"

using namespace std;

GlushkovAutomat::GlushkovAutomat(Node* source_tree): _source_tree(source_tree){}

Node* GlushkovAutomat::getSourceTree(){
    return _source_tree;
}

set create_la

int linearize_tree(Node* root, int count){
    if (root != nullptr){
        if (root ->getOp() == ops::LETTER){
            root ->setLetter(root -> getLetter() + to_string(count));
            count++;
        }
        for (auto elem: root-> getChildren()){
            count = linearize_tree(elem, count);
        } 
    }
    return count;
}

void GlushkovAutomat::linearizeTree(){
    linearize_tree(_source_tree, 0);
}

set create_lambda_set(){

}