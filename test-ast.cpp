#include <iostream>
#include <assert.h>
#include "ast.h"

void test_matom() {
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

  MATOM::destroy_symbols();
}

void test_mexpr() {
  // https://en.cppreference.com/w/c/language/struct_initialization
  MEXP *foo = new MEXP { { .atom = new MATOM("foo") }, MEXP_TYPE::matom_type };
  MEXP *bar = new MEXP { { .atom = new MATOM("bar") }, MEXP_TYPE::matom_type };
  MEXP *baz = new MEXP { { .atom = new MATOM("baz") }, MEXP_TYPE::matom_type };

  MEXPR *list = new MEXPR(foo);
  list->push_child(bar);
  list->push_child(baz);

  MEXP *expr = new MEXP { { .node = list }, MEXP_TYPE::mexpr_type };

  std::cout << "MATOM count: " << MATOM::get_count() << std::endl;
  assert(MATOM::get_count() == 3);

  std::cout << "expr: parent " << expr->val.node->get_parent()->val.atom->to_str() << " with " << expr->val.node->num_children() << " children\n";
  assert(expr->val.node->get_parent()->val.atom->to_str() == "foo");
  assert(expr->val.node->num_children() == 2);

  MEXPR *rlist = new MEXPR(bar);
  rlist->push_child(new MEXP { { .node = list }, MEXP_TYPE::mexpr_type });
  MEXP *rexpr = new MEXP { { .node = rlist }, MEXP_TYPE::mexpr_type };

  assert(MATOM::get_count() == 3);
  std::cout << "rexpr: parent " << rexpr->val.node->get_parent()->val.atom->to_str() << " with " << rexpr->val.node->num_children() << " children\n";
  assert(rexpr->val.node->get_parent()->val.atom->to_str() == "bar");
  assert(rexpr->val.node->num_children() == 1);
  std::cout << "  " << rexpr->val.node->to_str() << std::endl;
  assert(rexpr->val.node->to_str() == "(bar (foo bar baz))");

  MATOM::destroy_symbols();
}

void test_encloser() {
  assert(MATOM::is_encloser_string("[:"));
  assert(MATOM::is_encloser_string("|(!"));
  assert(MATOM::is_encloser_string("_{?"));
  assert(MATOM::is_encloser_string("<<"));
  assert(!MATOM::is_encloser_string("foo"));
  assert(MATOM::generate_closer("({[<") == ">]})");
  assert(MATOM::generate_closer("(ba:") == ":ab)");

  MATOM *enc = new MATOM("({");
  assert(enc->get_is_encloser());
  assert(enc->get_closer() == "})");

  MEXP *foo = new MEXP { { .atom = new MATOM("foo") }, MEXP_TYPE::matom_type };
  MEXP *bar = new MEXP { { .atom = new MATOM("bar") }, MEXP_TYPE::matom_type };
  MEXP *baz = new MEXP { { .atom = new MATOM("baz") }, MEXP_TYPE::matom_type };

  MEXPR *list = new MEXPR(new MEXP { { .atom = enc }, MEXP_TYPE::matom_type });
  list->push_child(foo);
  list->push_child(bar);
  list->push_child(baz);

  MEXP *expr = new MEXP { { .node = list }, MEXP_TYPE::mexpr_type };

  std::cout << "encloser expr: " << MEXP_TO_STR(expr) << "\n";
  assert(MEXP_TO_STR(expr) == "({ foo bar baz })"); 

  MATOM::destroy_symbols();
}

int main(int argc, char** argv) {
  test_matom();
  test_encloser();
  test_mexpr();

  return 0;
}

