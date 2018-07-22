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

void test_simple_list() {
  MEXP *el  = (new MexprBuilder("foo bar baz"))->parse();
  assert(MEXP_TO_STR(el) == "(foo bar baz)");

  MEXP *el2 = (new MexprBuilder("  {[  foo  bar  baz  ]}  "))->parse(false);
  assert(MEXP_TO_STR(el2) == "{[ foo bar baz ]}");

  MEXP *el3 = (new MexprBuilder("<:< hey  there >:>"))->parse(false);
  assert(MEXP_TO_STR(el3) == "<:< hey there >:>");
}

int main(int argc, char** argv) {
  test_atom();
  test_simple_list();

  return 0;
}

