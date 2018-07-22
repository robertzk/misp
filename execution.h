#ifndef execution_h
#define execution_h

#include "tokenizer.h"
#include <iostream>
#include <map>

class MispExecutionStrategy {
protected:
  MEXP *program;

public:
  MispExecutionStrategy(MEXP *_program) : program(_program) { }
  ~MispExecutionStrategy() { }

  virtual void execute() = 0;
  virtual void effect(MEXP *node, MEXP *sibling) = 0;
  virtual void finalize(MEXP *node, MEXP *sibling = NULL) = 0;
};

class LTRTraverseStrategy : public MispExecutionStrategy {
protected:

  void execute1(MEXP *node, MEXP *sibling = NULL);

public:
  LTRTraverseStrategy(MEXP *_program) : MispExecutionStrategy(_program) { }

  void execute();
};

class LTRAtomPrinterStrategy : public LTRTraverseStrategy {
private:
  std::ostream &os;

public:
  LTRAtomPrinterStrategy(MEXP *_program, std::ostream &_os = std::cout) : LTRTraverseStrategy(_program), os(_os) { }

  void execute();
  void effect(MEXP *node, MEXP *sibling = NULL);
  void finalize(MEXP *node, MEXP *sibling = NULL);
};

class MispBinding {
protected:
  typedef void(*MispLambda)(MEXP *, MEXP *, MispBinding *);

  std::map<std::string,MispLambda> binding_table;

public:

  MispBinding() { }
  void apply(MEXP *node, MEXP *sibling) {
    if (MEXP_IS_ATOM(node)) {
      auto it = this->binding_table.find(node->val.atom->to_str());
      if (it != this->binding_table.end()) { // Bound lambda
        (*it->second)(node, sibling, this);
      }
    }
  }
};

class PrintBinding : public MispBinding {
private:
  std::ostream &os;

  void initialize_table() {
    binding_table["print"] = &PrintBinding::print;
  }

  static void print(MEXP* node, MEXP* sibling, MispBinding *binding) {
    if (MEXP_IS_ATOM(node) && node->val.atom->to_str() == "print") {
      if (sibling != NULL) {
        (static_cast<PrintBinding*>(binding))->get_os() << MEXP_TO_STR(sibling);
      }
    }
  }

public:
  PrintBinding(std::ostream &_os = std::cout) : MispBinding(), os(_os) {
    initialize_table();
  }

  std::ostream &get_os() { return this->os; }
};

class ApplyBindingLTRStrategy : public LTRTraverseStrategy {
private:
  MispBinding *binding;

public:

  ApplyBindingLTRStrategy(MEXP *_program, MispBinding *_binding = NULL) : LTRTraverseStrategy(_program) {
    if (_binding == NULL) {
      this->binding = new PrintBinding();
    } else {
      this->binding = _binding;
    }
  }

  void effect(MEXP *node, MEXP *sibling = NULL);
  void finalize(MEXP *node, MEXP *sibling = NULL);
};


#endif

