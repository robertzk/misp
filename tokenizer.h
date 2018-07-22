#ifndef tokenizer_h
#define tokenizer_h

#include "ast.h"

class MexprBuilder {
private:
  std::string source;

  static bool is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
  }

public:
  MexprBuilder(std::string s) : source(s) { }

  MEXP* parse(bool default_parentheses = false) {
    for (auto ch : source ) {
      if (is_whitespace(ch)) { continue; }

    }
    return new MEXP { { .atom = new MATOM("foo") }, MEXP_TYPE::matom_type };
  } 

};

#endif

