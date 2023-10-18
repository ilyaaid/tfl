#include "parser.hpp"
#include "glushkov.hpp"
#include <ctime> 

#include <iostream>
#include <exception>
#include <set>
#include <algorithm>
#include <regex>

using namespace std;


int main(int, char **)
{   
    // string str = "aaaabb";
    // regex word_regex ("a*bbb");
    // cout << regex_match(str, word_regex);
    string regExpr = "";
    getline(cin, regExpr);
    RegExprParser parser;
    Node* root = nullptr;
    try
    {
        root = parser.parse(regExpr, "abcde?");
        //cout << root->getString() << endl;
        //упрощение регулярки
        root = root->simple();
        string newRegExpr = root -> getString();
        regex word_regex (regExpr);
        regex word_regex_new (newRegExpr);
        cout << "new regex: " << newRegExpr << endl;
        cout << "old regex: " << regExpr << endl;
        //построение автомата
        GlushkovAutomat automat(root);
        automat.linearizeTree();
        automat.findPSet();
        automat.findDSet();
        automat.findFSet();
        automat.make_automat();
        //построение слов для проверки 
        vector<Path> words = automat.create_words();
        vector<string> res_words = automat.make_strings(words);
        //цикл на проверку строк 
        for (auto elem: res_words){
            if (regex_match(elem, word_regex)){
                cout << "ok" << endl;
            } else {
                cout << "fail: " << elem << endl; 
            }
            //замеряем время на новой регулярке
            clock_t start, end;
            double seconds;
            start = clock();
            regex_match(elem, word_regex_new);
            end = clock();
            seconds = (double)(end - start) / CLOCKS_PER_SEC;
            printf("new regex: %f seconds\n", seconds);
            //замеряем время на старой регулярке
            start = clock();
            regex_match(elem, word_regex);
            end = clock();
            seconds = (double)(end - start) / CLOCKS_PER_SEC;
            printf("old regex: %f seconds\n", seconds);
            
        }
        //automat.print_automat(); 
        cout << "done!" << endl;
        delete root;
    }
    catch (const exception &err)
    {
        if (root) {
            delete root;
        }
        cout << err.what() << endl;
    }
    cout << endl;
    return 0;
}
