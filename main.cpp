#include "parser.hpp"
#include "regexpr.hpp"

#include <iostream>
#include <exception>


using namespace std;

int main(int, char**){
    string regExpr = "";
    getline(cin, regExpr);
    RegExprParser parser;
    try {
        RegExprShPtr root = parser.parse(regExpr, "abc");
        root.get()->output();
    } catch(const exception& err) {
        cout << err.what() << endl;
    }

    cout << endl;
    return 0;
}
