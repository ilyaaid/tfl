#pragma once

#include "regexpr.hpp"

#include <string>

using namespace std;

class RegExprParser {
  public:
  RegExprShPtr parse(const string& regexpr, const string& alphabet);

  private:
  RegExprShPtr parseExpr();
  RegExprShPtr parseT1();
  RegExprShPtr parseT2();

  private:
  bool isInAlpabet(char c);
  static bool isSpace(unsigned char c);

  private:
  string regexpr_;
  string alphabet_;
  int cur_i_;
};
