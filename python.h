#ifndef python_h
#define python_h

#include "strategies.h"

class PythonExecutionBinding : public MispBinding {
protected:
  std::ostream &os;
  std::stringstream pyprogram;
  int skipcount;

  void initialize_table();

  // These are based off the EBNF grammar
  // https://docs.python.org/2.7/reference/grammar.html
  static void import(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void apply_method(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void make_string(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void make_tuple(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void make_list(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void make_dict(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void call_function(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void assign(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void element(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void print(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void define_function(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void for_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void andor_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void in_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void return_statement(MEXP* node, MEXP* sibling, MispBinding *binding);

  // TODO: Implement these
  /*
  static void arithmetic_operator(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void comparison_operator(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void binary_operator(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void unary_operator(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void arithmetic_term(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void slice(MEXP* node, MEXP* sibling, MispBinding *binding);

  static void make_backtick(MEXP* node, MEXP* sibling, MispBinding *binding);

  static void decorator(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void define_class(MEXP* node, MEXP* sibling, MispBinding *binding);

  static void del_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void pass_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void break_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void continue_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void yield_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void raise_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void from_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void global_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void exec_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void assert_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void if_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void while_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void try_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void with_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void lambda_expression(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void or_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void and_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void not_statement(MEXP* node, MEXP* sibling, MispBinding *binding);
  */

  static void unknown_binding(MEXP* node, MEXP* sibling, MispBinding *binding);


public:
  PythonExecutionBinding(std::ostream &_os = std::cout) : MispBinding(), os(_os), skipcount(0) { initialize_table(); }

  void apply(MEXP *node, MEXP *sibling);

  std::ostream &get_os() { return this->os; }
  std::stringstream &get_pyprogram() { return this->pyprogram; }

  void increment_skipcount() { this->skipcount++; }
  int get_skipcount() { return this->skipcount; }
};


class PythonExecutionStrategy : public ApplyBindingLTRStrategy {
protected:
  std::ostream &pystream;
  std::string filename;

public:
  PythonExecutionStrategy(MEXP *_program, std::ostream &_os = std::cout, std::ostream &_pystream = std::cout, std::string _filename = "") : 
    ApplyBindingLTRStrategy(_program, new PythonExecutionBinding(_os)), pystream(_pystream), filename(_filename) { }

  void execute();
  void effect(MEXP *node, MEXP *sibling = NULL);
  void finalize(MEXP *node, MEXP *sibling = NULL);
};

#endif

