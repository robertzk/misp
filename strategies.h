#ifndef strategies_h
#define strategies_h

#include "execution.h"

bool is_encloser_atom(MEXP *node);
bool is_symbolic_atom(MEXP *node);
bool is_atomic_expression(MEXP *node);

class ShellExecutionBinding : public MispBinding {
protected:
  std::ostream &os;

  void initialize_table();
  static void exec(MEXP* node, MEXP* sibling, MispBinding *binding);

public:
  ShellExecutionBinding(std::ostream &_os = std::cout) : MispBinding(), os(_os) { initialize_table(); }

  std::ostream &get_os() { return this->os; }

  static std::string shell_exec(std::string cmd);
};


class ShellExecutionStrategy : public ApplyBindingLTRStrategy {
public:
  ShellExecutionStrategy(MEXP *_program, std::ostream &_os = std::cout) : 
    ApplyBindingLTRStrategy(_program, new ShellExecutionBinding(_os)) {
  }

  void finalize(MEXP *node, MEXP *sibling = NULL);
};


class PythonExecutionBinding : public MispBinding {
protected:
  std::ostream &os;
  std::stringstream pyprogram;
	int skipcount;

  void initialize_table();
  static void import(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void apply_function(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void make_string(MEXP* node, MEXP* sibling, MispBinding *binding);
  static void print(MEXP* node, MEXP* sibling, MispBinding *binding);
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

