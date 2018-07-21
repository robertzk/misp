#include "misp.h"

int main(int argc, char** argv) {
  MATOM *m = new MATOM("foo");

  std::cout << "MISP! " << m->to_str();

  return 0;
}

