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
};

class LTRTraverseStrategy : public MispExecutionStrategy {
protected:

  virtual void execute1(MEXP *tree, MEXP *sibling = NULL) = 0;

public:
  LTRTraverseStrategy(MEXP *_program) : MispExecutionStrategy(_program) { }

  void execute() {
    if (MEXP_IS_ATOM(this->program)) {
      execute1(this->program);
    } else {
      // Parent -> children tree walk
      execute1(this->program->val.node->get_parent());
      auto children = this->program->val.node->get_children();
      for (unsigned int i = 0; i < children->size(); i++) {
        auto child = children->at(i);
        auto sibling = child;
        if (i == children->size() - 1) {
          sibling = NULL;
        } else {
          sibling = children->at(i + 1);
        }
        // TODO: Should we pass the full tail here? Or maybe just let do a SIBLING macro?
        execute1(child, sibling);
      }
    }
  }
};

class LTRAtomPrinterStrategy : public LTRTraverseStrategy {
protected:
  void execute1(MEXP *tree, MEXP *sibling = NULL) {
    if (MEXP_IS_ATOM(tree)) {
      effect(tree);
    }
  }

public:

  void execute() {
    std::cout << "Executing: \n";
    LTRTraverseStrategy::execute();
    std::cout << "\nExecution completed.\n";
  }

  void effect(MEXP *node) {
    std::cout << "Atom: " << MEXP_TO_STR(node) << std::endl;
  }
};

#endif

