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
  bool is_encloser;
  std::string closer;

  MATOM(int i);

public:

  MATOM(std::string s);
  int get_index() const;
  std::string get_closer() const;
  bool get_is_encloser() { return this->is_encloser; }; 
  bool encloser() { return this->is_encloser; }
  std::string to_str() const;

  static int get_count() { return count; };
  static void destroy_symbols();
  static bool is_encloser_char(char ch);
  static bool is_encloser_body_char(char ch);
  static bool is_closer_char(char ch);
  static bool is_encloser_string(std::string s);
  static bool is_closer_string(std::string s);
  static std::string generate_closer(std::string s);
};

class MEXPR {
private:
  MEXP *parent;
  std::vector<MEXP*> children;

public:
  MEXPR(MEXP *_parent);
  bool push_child(MEXP* child);

  MEXP *get_parent() { return this->parent; }
  std::vector<MEXP*>* get_children() { return &this->children; }
  int num_children() const { return this->children.size(); }

  std::string to_str() const;
};

#endif

