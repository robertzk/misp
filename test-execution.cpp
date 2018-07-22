#include <iostream>
#include <sstream>
#include <assert.h>
#include <string>
#include "execution.h"

void assert_contains(std::string s, std::string sub) {
  assert(s.find(sub) != std::string::npos);
}

void test_trivial_execution() {
  std::stringstream ss;
  MEXP *program = (new MexprBuilder("foo [bar baz] bux"))->parse();
  LTRAtomPrinterStrategy(program, ss).execute();
  std::string s = ss.str();
  char const *substrings[7] = {"Atom: (", "Atom: [", "Atom: bar", "Atom: baz", 
                               "Finalizing: [", "Atom: bux", "Finalizing: ("};
	for (unsigned int i = 0; i < 7; i++) {
    assert_contains(s, std::string(substrings[i]));
	}
}

int main(int argc, char** argv) {
  test_trivial_execution();

  return 0;
}

