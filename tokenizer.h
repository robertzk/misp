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

  MEXP* parse_list(std::vector<std::string> tokens, unsigned int &offset);

public:
  MexprBuilder(std::string s) : source(s) { }
  MEXP* parse(bool default_parentheses = true);
  std::vector<std::string> tokenize(bool default_parentheses);
};

#endif

