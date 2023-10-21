#include "parser.hpp"
#include "glushkov.hpp"
#include <ctime> 

#include <iostream>
#include <exception>
#include <set>
#include <algorithm>
#include <regex>

using namespace std;


int rand_int(int start, int end){
    return rand() % (end - start + 1) + start;
}

char rand_letter (string alphabet){
    return alphabet[rand_int(0, alphabet.length() - 1)];
}

char rand_op(string ops){
    return ops[rand_int(0, ops.length() - 1)];
}

string gen_regex(int max_depth, int cur_depth, string alphabet, string ops){
    string res = "";
    if (cur_depth == max_depth){
        char op = rand_op(ops);
        if (op == '*'){
            res.push_back('(');
            res.push_back(rand_letter(alphabet));
            res.push_back(')');
            res.push_back('*');
        }else if (op == 'c'){
            res.push_back(rand_letter(alphabet));
            res.push_back(rand_letter(alphabet));
        }else{
            res.push_back(rand_letter(alphabet));
            res.push_back('|');
            res.push_back(rand_letter(alphabet));
        }
    }else{
        char op = rand_op(ops);
        if (op == '*'){
            res.push_back('(');
            res += gen_regex(max_depth, cur_depth + 1, alphabet, ops);
            res.push_back(')');
            res.push_back('*');
        }else if (op == 'c'){
            res += gen_regex(max_depth, cur_depth + 1, alphabet, ops);
            res += gen_regex(max_depth, cur_depth + 1, alphabet, ops);
        }else{
            res += gen_regex(max_depth, cur_depth + 1, alphabet, ops);
            res.push_back('|');
            res += gen_regex(max_depth, cur_depth + 1, alphabet, ops);
        }
    }
    return res;
}

int main(int, char **)
{   
    srand(time(NULL));

    int n_regexes;
    cout << "ВВедите количество регулярок которых нужно протестировать" << endl;
    cin >> n_regexes;
    for (int i = 0; i < n_regexes; i++){
        string regExpr = gen_regex(3, 0, "abcde", "*|c");
        RegExprParser parser;
        Node* root = nullptr;
        try
        {
            root = parser.parse(regExpr, "abcde");
            //cout << root->getString() << endl;
            //упрощение регулярки
            root = root->simple();
            string newRegExpr = root -> getString();
            regex word_regex (regExpr);
            regex word_regex_new (newRegExpr);
            cout << "old regex: " << regExpr << endl;
            cout << "new regex: " << newRegExpr << endl;
            //построение автомата
            GlushkovAutomat automat(root);
            automat.linearizeTree();
            automat.findPSet();
            automat.findDSet();
            automat.findFSet();
            automat.make_automat();
            //построение слов для проверки 
            vector<Path> words = automat.create_words();
            //цикл на проверку строк 
            cout << "cnt tests: " << words.size() << endl;
            //количество пройденных тестов
            int oks = 0;
            //количество проваленных тестов
            int fails = 0;
            for (int i = 0; i < words.size(); ++i){
                string str = words[i].make_string();
                //cout << "ind: " << i << endl;
                // cout << "test string: " << str << endl;
                if (regex_match(str, word_regex_new)){
                    oks ++;
                } else {
                    cout << "fail " << str << endl;
                    fails ++;
                }
                //замеряем время на новой регулярке
                clock_t start, end;
                double seconds;
                start = clock();
                regex_match(str, word_regex_new);
                end = clock();
                seconds = (double)(end - start) / CLOCKS_PER_SEC;
                //printf("new regex: %f seconds\n", seconds);
                //замеряем время на старой регулярке
                start = clock();
                regex_match(str, word_regex);
                end = clock();
                seconds = (double)(end - start) / CLOCKS_PER_SEC;
                //printf("old regex: %f seconds\n", seconds);
                
            }
            cout << "oks " << oks << endl;
            cout << "fails " << fails << endl;
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
    }
    return 0;
}
