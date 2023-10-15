#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include "glushkov.hpp"

using namespace std;


set<string>multiply_to_lambda(set<string> set1, bool lambda){
    set<string>empty_set;
    if (lambda){
        return set1;
    }else{
        return empty_set;
    }
}

set<pair_of_vertices, Cmp> pair_multiply(set<string> set1, set<string> set2){
    set<pair_of_vertices, Cmp>res;
    for (auto elem1: set1){
        for (auto elem2: set2){
            res.insert(pair_of_vertices(elem1, elem2));
        }
    }
    return res;
}


set<string> sets_union(set<string> set1, set<string> set2){
    for (auto elem: set1){
        set2.insert(elem);
    }
    return set2;
}

set<pair_of_vertices, Cmp> pair_sets_union(set<pair_of_vertices, Cmp> set1, set<pair_of_vertices, Cmp> set2){
    for (auto elem: set1){
        set2.insert(elem);
    }
    return set2;
}

set<string> sets_intersection(set<string> set1, set<string> set2){
    set<string>res;
    set_intersection(begin(set1), end(set1), begin(set2), end(set2), inserter(res, begin(res)));
    return res;
}

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


GlushkovAutomat::GlushkovAutomat(Node* source_tree): _source_tree(source_tree){}

Node* GlushkovAutomat::getSourceTree(){
    return _source_tree;
}


void GlushkovAutomat::linearizeTree(){
    linearize_tree(_source_tree, 1);
}



bool find_lambda(Node* node){
    if (node -> getOp() == ops::CONCAT){
    for (auto elem : node -> getChildren()){
        if (!find_lambda(elem)){
            return false;
        }
    }
    return true;
    }
    if (node -> getOp() == ops::OR){
    for (auto elem : node -> getChildren()){
        if (find_lambda(elem)){
            return true;
        }
    }
    return false;
    }
    if (node -> getOp() == ops::STAR){
    return true;
    }
    if (node -> getOp() == ops::LETTER){
    return false;
    }
    return false;
}



set<string> find_p_set(Node* parent,  vector<Node*>children){
    if (parent -> getOp() == ops::OR){
        Node* last_operand = children.back();
        if (children.size() == 1){
            return find_p_set(last_operand, last_operand ->getChildren());
        }
        children.pop_back();
        return sets_union(find_p_set(parent, children), 
        find_p_set(last_operand, last_operand -> getChildren()));
    }
    if (parent -> getOp() == ops::CONCAT){
        Node* last_operand = children.back();
        if (children.size() == 1){
            return find_p_set(last_operand, last_operand ->getChildren());
        }
        children.pop_back();
        return sets_union(find_p_set(parent, children), 
        multiply_to_lambda(find_p_set(last_operand, last_operand -> getChildren()), 
        find_lambda(parent)));
    }
    if (parent->getOp() == ops::STAR){
        Node* last_operand = children.back();
        return find_p_set(last_operand, last_operand -> getChildren());
    }
    if (parent->getOp() == ops::LETTER){
        string str = parent->getLetter();
        set<string> res {str};
        return res;
    }
    return set<string>{};
}

void GlushkovAutomat::findPSet(){
    _p_set = find_p_set(_source_tree, _source_tree -> getChildren());
}

set<string> GlushkovAutomat::getPSet(){
    return _p_set;
}

set<string> find_d_set(Node* parent,  vector<Node*>children){
    if (parent -> getOp() == ops::OR){
        Node* last_operand = children.back();
        if (children.size() == 1){
            return find_d_set(last_operand, last_operand ->getChildren());
        }
        children.pop_back();
        return sets_union(find_d_set(parent, children), 
        find_d_set(last_operand, last_operand -> getChildren()));
        
    }
    if (parent -> getOp() == ops::CONCAT){
        Node* last_operand = children.back();
        if (children.size() == 1){
            return find_d_set(last_operand, last_operand ->getChildren());
        }
        children.pop_back();
        return sets_union(find_d_set(last_operand, last_operand->getChildren()), 
        multiply_to_lambda(find_d_set(parent, children), find_lambda(last_operand)));
        
    }
    if (parent->getOp() == ops::STAR){
        Node* last_operand = children.back();
        return find_d_set(last_operand, last_operand -> getChildren());
    }
    if (parent->getOp() == ops::LETTER){
        string str = parent->getLetter();
        set<string> res {str};
        return res;
    }
    return set<string>{};
}

void GlushkovAutomat::findDSet(){
    _d_set = find_d_set(_source_tree, _source_tree -> getChildren());
}

set<string>GlushkovAutomat::getDSet(){
    return _d_set;
}

// set<pair_of_vertices, Cmp> find_f_set(Node* node){
//     if (node -> getOp() == ops::OR){
//         set<pair_of_vertices, Cmp> res;
//         for (int i = 0; i < node->getChildren().size(); i++){
//             res = pair_sets_union(res, find_f_set(node->getChildren()[i]));
//         }
//     }
//     if (node -> getOp() == ops::CONCAT){
//         set<pair_of_vertices, Cmp> f_res = find_f_set(node->getChildren()[0]);
//         set<string> d_res = find_d_set(node -> getChildren()[0], node -> getChildren()[0] -> getChildren());
//         for (int i = 1; i < node->getChildren().size(); i++){
//             f_res = pair_sets_union(f_res, find_f_set(node->getChildren()[i]));
//             f_res = pair_sets_union(f_res, pair_multiply(d_res, find_p_set(node -> getChildren()[i], node -> getChildren()[i] -> getChildren() )));
//         }
//     }
//     if (node -> getOp() == ops::STAR){

//     }
// }

set<pair_of_vertices, Cmp>  find_f_set(Node* parent,  vector<Node*>children){
    if (parent -> getOp() == ops::OR){
        Node* last_operand = children.back();
        if (children.size() == 1){
            return find_f_set(last_operand, last_operand ->getChildren());
        }
        children.pop_back();
        return pair_sets_union(find_f_set(parent, children), 
        find_f_set(last_operand, last_operand -> getChildren()));

    }
    if (parent -> getOp() == ops::CONCAT){
        Node* last_operand = children.back();
        if (children.size() == 1){
            return find_f_set(last_operand, last_operand ->getChildren());
        }
        children.pop_back();
        return pair_sets_union(find_f_set(parent, children), 
        pair_sets_union(find_f_set(last_operand, last_operand->getChildren()),
        pair_multiply(find_d_set(parent, children),
        find_p_set(last_operand, last_operand->getChildren()))));
        
    }
    if (parent->getOp() == ops::STAR){
        Node* last_operand = children.back();
        return pair_sets_union(find_f_set(last_operand, last_operand->getChildren()), 
        pair_multiply(find_d_set(last_operand, last_operand -> getChildren()),
        find_p_set(last_operand, last_operand -> getChildren())));
    }
    if (parent->getOp() == ops::LETTER){
        return set<pair_of_vertices, Cmp>{};
    }
    return set<pair_of_vertices, Cmp>{};
}

void GlushkovAutomat::findFSet(){
    _f_set = find_f_set(_source_tree, _source_tree -> getChildren());
}

set<pair_of_vertices, Cmp>GlushkovAutomat::getFSet(){
    return _f_set;
}





