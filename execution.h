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
  void apply(MEXP *node, MEXP *sibling);
};

class PrintBinding : public MispBinding {
private:
  std::ostream &os;

  void initialize_table();
  static void print(MEXP* node, MEXP* sibling, MispBinding *binding);

public:
  PrintBinding(std::ostream &_os = std::cout) : MispBinding(), os(_os) { initialize_table(); }

  std::ostream &get_os() { return this->os; }
};

class TokenDebugBinding : public MispBinding {
private:
  std::ostream &os;

  void initialize_table();
  static void print_token(MEXP* node, MEXP* sibling, MispBinding *binding);

public:
  TokenDebugBinding(std::ostream &_os = std::cout) : MispBinding(), os(_os) { initialize_table(); }

  std::ostream &get_os() { return this->os; }
};

class ApplyBindingLTRStrategy : public LTRTraverseStrategy {
protected:
  MispBinding *binding;

public:
  ApplyBindingLTRStrategy(MEXP *_program, MispBinding *_binding = NULL);

  void effect(MEXP *node, MEXP *sibling = NULL);
  void finalize(MEXP *node, MEXP *sibling = NULL);
};

#endif

