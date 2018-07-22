#ifndef execution_h
#define execution_h

#include "tokenizer.h"

class MispExecutionStrategy {

public:
  MispExecutionStrategy() { }
  ~MispExecutionStrategy() { }
};

class LTRTraverseStrategy : public MispExecutionStrategy {

public:
  LTRTraverseStrategy() : MispExecutionStrategy() {

  }
};

#endif

