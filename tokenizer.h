#ifndef tokenizer_h
#define tokenizer_h

#include "ast.h"

class MexprBuilder {
private:

  std::string source;

public:
  MexprBuilder(std::string s) : source(s) { }
};

#endif

