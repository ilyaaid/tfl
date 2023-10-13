#include "parser.hpp"

#include <iostream>
#include <algorithm>
#include <exception>
#include <sstream>

using namespace std;

bool RegExprParser::isInAlpabet(char c)
{
    return alphabet_.find(c) < alphabet_.size();
}

bool RegExprParser::isSpace(unsigned char c)
{
    return (c == ' ' || c == '\n' || c == '\r' ||
        c == '\t' || c == '\v' || c == '\f');
}

RegExprShPtr RegExprParser::parse(const string& regexpr, const string& alphabet) {
    cur_i_ = 0;
    regexpr_ = regexpr;
    alphabet_ = alphabet;
    // удаляем пробелы
    regexpr_.erase(std::remove_if(regexpr_.begin(), regexpr_.end(), isSpace), regexpr_.end());
    
    RegExprShPtr root = parseExpr();
    if (!root) {
        throw runtime_error("null root : wrong regexpr");
    }
    if (cur_i_ != regexpr_.size()) {
        stringstream strm;
        strm << "wrong input in ";
        strm << cur_i_;
        throw runtime_error(strm.str());
    }
    return root;
}

RegExprShPtr RegExprParser::parseExpr()
{
    if (cur_i_ == regexpr_.size()) {
        return nullptr;
    }
    RegExprShPtr T1 = parseT1();
    if (cur_i_ == regexpr_.size()) {
        return T1;
    }
    char cur_el = regexpr_[cur_i_];
    if (cur_el == '|') {
        if (!T1) {
            throw runtime_error("null operand for '|'");
        }
        cur_i_++;
        RegExprShPtr expr = parseExpr();
        return make_shared<Or>(T1, expr);
    }
    RegExprShPtr expr = parseT1();
    if (expr) {
        return make_shared<Conc>(T1, expr);
    } else {
        return T1;
    }
}

RegExprShPtr RegExprParser::parseT1()
{
    if (cur_i_ == regexpr_.size()) {
        return nullptr;
    }
    RegExprShPtr T2 = parseT2();
    char cur_el = regexpr_[cur_i_];
    if (cur_el == '*') {
        while (cur_i_ < regexpr_.size() && regexpr_[cur_i_] == '*') {
            cur_i_++;
        }
        if (!T2) {
            throw runtime_error("null operand for '*'");
        }
        return make_shared<Star>(T2);
    }
    return T2;
}

RegExprShPtr RegExprParser::parseT2()
{
    if (cur_i_ == regexpr_.size()) {
        return nullptr;
    }
    char cur_el = regexpr_[cur_i_];
    if (cur_el == '(') {
        cur_i_++;
        RegExprShPtr expr = parseExpr();
        if (cur_i_ >= regexpr_.size() || regexpr_[cur_i_] != ')') {
            throw runtime_error("error in parseT1: expected ')'");
        }
        cur_i_++;
        return expr;
    }
    if (isInAlpabet(cur_el)) {
        string concStr = "";
        concStr.push_back(cur_el);
        cur_i_++;
        while (cur_i_ + 1 < regexpr_.size() 
        && isInAlpabet(regexpr_[cur_i_])
        && isInAlpabet(regexpr_[cur_i_ + 1])) {
            concStr.push_back(regexpr_[cur_i_]);
            cur_i_++;
        }
        return make_shared<ConcLit>(concStr);
    }
    return nullptr;
}
