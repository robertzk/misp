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
  void effect(MEXP *node);
  void finalize(MEXP *node, MEXP *sibling = NULL);
};

#endif

