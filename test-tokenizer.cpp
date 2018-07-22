#include <iostream>
#include <assert.h>
#include "tokenizer.h"

void test_atom() {
  // Trivial atom parsing
  MEXP *el  = (new MexprBuilder("blah"))->parse();
  MEXP *el2 = (new MexprBuilder("(blah)"))->parse(false);

  assert(MEXP_TO_STR(el) == "(blah)");
  assert(MEXP_TO_STR(el2) == "(blah)");
}

int main(int argc, char** argv) {
  test_atom();

  return 0;
}

