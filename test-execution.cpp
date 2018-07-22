#include <iostream>
#include <sstream>
#include <assert.h>
#include "execution.h"

void test_trivial_execution() {
  // Fill in
  std::stringstream ss;
  MEXP *program = (new MexprBuilder("foo [bar baz] bux"))->parse();
  LTRAtomPrinterStrategy(program, ss).execute();
  std::cout << ss.str();
}

int main(int argc, char** argv) {
  test_trivial_execution();

  return 0;
}

