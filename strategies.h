#ifndef strategies_h
#define strategies_h

#include "execution.h"

bool is_atom(MEXP *node);
bool is_encloser_atom(MEXP *node);
bool is_symbolic_atom(MEXP *node);

bool is_square_bracket_expression(MEXP *node);
bool is_brace_bracket_expression(MEXP *node);
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


class DebugTokensStrategy : public ApplyBindingLTRStrategy {
public:
  DebugTokensStrategy(MEXP *_program, std::ostream &_os = std::cout) : 
    ApplyBindingLTRStrategy(_program, new TokenDebugBinding(_os)) {
  }

  void effect(MEXP *node, MEXP *sibling = NULL);
  void finalize(MEXP *node, MEXP *sibling = NULL);
};

#endif

