#ifndef strategies_h
#define strategies_h

#include "execution.h"

class ShellExecutionBinding : public MispBinding {
protected:
  std::ostream &os;

  void initialize_table();
  static void exec(MEXP* node, MEXP* sibling, MispBinding *binding);

  static bool is_encloser_atom(MEXP *node);
  static bool is_symbolic_atom(MEXP *node);
  static bool is_shell_expression(MEXP *node);

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

#endif

