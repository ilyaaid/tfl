#include <iostream>
#include <set>
#include "node.hpp"

using namespace std;



struct pair_of_vertices{
    string _x,_y;
    pair_of_vertices(string x, string y):_x(x), _y(y){}
};

class Cmp{
    public:
    bool operator()( const pair_of_vertices& pair1, const pair_of_vertices& pair2) const {
        if (pair1._x < pair2._x){
            return true;
        }
        if (pair1._y < pair2._y){
            return true;
        }
        return false;
    }
};

class GlushkovAutomat{
private:
    Node* _source_tree;
    set<string>_p_set;
    set<string>_d_set;
    set<pair_of_vertices, Cmp>_f_set;


public:
    GlushkovAutomat (Node* source_tree);
    Node* getSourceTree();
    void linearizeTree();
    void findPSet();
    set<string>getPSet();
    void findDSet();
    set<string>getDSet();
    void findFSet();
    set<pair_of_vertices, Cmp>getFSet();
};

