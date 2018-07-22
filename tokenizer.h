#ifndef tokenizer_h
#define tokenizer_h

#include "ast.h"

class MexprBuilder {
private:
  std::string source;

public:
  MexprBuilder(std::string s) : source(s) { }

  MEXP* parse(bool default_parentheses = false) {
    return new MEXP { { .atom = new MATOM("foo") }, MEXP_TYPE::matom_type };
  } 
};

#endif

