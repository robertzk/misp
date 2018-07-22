#ifndef execution_h
#define execution_h

#include "tokenizer.h"
#include <iostream>

class MispExecutionStrategy {
protected:
  MEXP *program;

public:
  MispExecutionStrategy(MEXP *_program) : program(_program) { }
  ~MispExecutionStrategy() { }

  virtual void execute() = 0;
  virtual void effect(MEXP *node) = 0;
  virtual void finalize(MEXP *node, MEXP *sibling = NULL) = 0;
};

class LTRTraverseStrategy : public MispExecutionStrategy {
protected:

  void execute1(MEXP *node, MEXP *sibling = NULL) {
    if (MEXP_IS_ATOM(node)) {
      effect(node);
    } else {
      // Parent -> children tree walk
      execute1(node->val.node->get_parent());
      auto children = node->val.node->get_children();
      for (unsigned int i = 0; i < children->size(); i++) {
        auto child = children->at(i);
        auto sibling_node = child;
        if (i == children->size() - 1) {
          sibling_node = NULL;
        } else {
          sibling_node = children->at(i + 1);
        }
        // TODO: Should we pass the full tail here? Or maybe just let do a SIBLING macro?
        execute1(child, sibling_node);
      }
      finalize(node, sibling);
    }
  }

public:
  LTRTraverseStrategy(MEXP *_program) : MispExecutionStrategy(_program) { }

  void execute() {
    execute1(this->program);
  }
};

class LTRAtomPrinterStrategy : public LTRTraverseStrategy {
private:
  std::ostream &os;

public:
  LTRAtomPrinterStrategy(MEXP *_program, std::ostream &_os = std::cout) : LTRTraverseStrategy(_program), os(_os) { }

  void execute() {
    os << "Executing: \n";
    LTRTraverseStrategy::execute();
    os << "\nExecution completed.\n";
  }

  void effect(MEXP *node) {
    if (MEXP_IS_ATOM(node)) {
      os << "Atom: " << MEXP_TO_STR(node) << std::endl;
    }
  }

  void finalize(MEXP *node, MEXP *sibling = NULL) {
    if (MEXP_IS_EXPR(node)) {
      os << "Finalizing: " << MEXP_TO_STR(node->val.node->get_parent()) << std::endl;
    }
  }
};

#endif

