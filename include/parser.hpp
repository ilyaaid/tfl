#pragma once

#include "node.hpp"

#include <string>

using namespace std;

class RegExprParser
{
public:
  Node *parse(const string &regexpr, const string &alphabet);

private:
  bool isInAlpabet(char c);
  static bool isSpace(char c);
  void initParams(const string &regexpr, const string &alphabet);

private:
  string regexpr_;
  string alphabet_;
  int cur_i_;
};
