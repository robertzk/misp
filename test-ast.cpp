#include <iostream>
#include <assert.h>
#include "ast.h"

int main(int argc, char** argv) {
  MATOM *foo  = new MATOM("foo");
  MATOM *foo2 = new MATOM("foo");
  MATOM *bar  = new MATOM("bar");

  std::cout << "MATOM count: " << MATOM::get_count() << std::endl;
  assert(MATOM::get_count() == 2);
  std::cout << "Foo:  (" << foo->to_str() << ", " << foo->get_index() << ")\n";
  assert(foo->to_str() == "foo"); assert(foo->get_index() == 0);
  std::cout << "Foo2: (" << foo2->to_str() << ", " << foo2->get_index() << ")\n";
  assert(foo2->to_str() == "foo"); assert(foo2->get_index() == 0);
  std::cout << "Bar:  (" << bar->to_str() << ", " << bar->get_index() << ")\n";
  assert(bar->to_str() == "bar"); assert(bar->get_index() == 1);

  return 0;
}

