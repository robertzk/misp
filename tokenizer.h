#ifndef tokenizer_h
#define tokenizer_h

#include "ast.h"
#include <vector>
#include <string>
#include <stack>

class MexprBuilder {
private:
  std::string source;

  static bool is_whitespace(char c);

public:
  MexprBuilder(std::string s) : source(s) { }
  MEXP* parse(bool default_parentheses = false);
  std::vector<std::string> tokenize(bool default_parentheses);
};

#endif

