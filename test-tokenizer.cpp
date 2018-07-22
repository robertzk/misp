#include <iostream>
#include <assert.h>
#include "tokenizer.h"

void test_simple_token() {
  // Trivial atom parse
  assert((new MexprBuilder("blah"))->parse()->val.atom->to_str() == "blah");
}

int main(int argc, char** argv) {
  test_simple_token();

  return 0;
}

