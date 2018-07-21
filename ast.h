#ifndef ast_h
#define ast_h

#include <stdio.h>
#include <string>
#include <map>

class MATOM;
class MEXPR;

union MEXP {
  MEXPR *node;
  MATOM *atom;
};

class MATOM {
private:

  static int count;
  static std::map<std::string,int> map;

  int index;

  MATOM(int i);

public:

  MATOM(std::string s);
  int get_index() const;
  std::string to_str() const;

  static int get_count() { return count; };
};

class MEXPR {

};

#endif

