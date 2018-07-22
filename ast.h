#ifndef ast_h
#define ast_h

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>

class MATOM;
class MEXPR;

enum MEXP_TYPE { mexpr_type, matom_type };
typedef union MEXP_UNION {
  MEXPR *node;
  MATOM *atom;
} MEXP_UNION;
struct MEXP {
  MEXP_UNION val;
  MEXP_TYPE val_type;
};

#define MEXP_IS_EXPR(x) ( (x)->val_type == MEXP_TYPE::mexpr_type )
#define MEXP_IS_ATOM(x) ( (x)->val_type == MEXP_TYPE::matom_type )
#define MEXP_TO_STR(x) ( (MEXP_IS_EXPR(x)) ? (x)->val.node->to_str() : (x)->val.atom->to_str() )

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
  static void destroy_symbols();
};

class MEXPR {
private:
  MEXP *parent;
  std::vector<MEXP*> children;

public:
  MEXPR(MEXP *_parent);
  bool push_child(MEXP* child);

  MEXP *get_parent() { return this->parent; }
  int num_children() const { return this->children.size(); }

  std::string to_str() const {
    std::stringstream ss;
    ss << '(' << MEXP_TO_STR(parent);

    for (auto child : this->children) {
      ss << ' ' << MEXP_TO_STR(child);
    }
    ss << ')';
    return ss.str();
  }
};

#endif

