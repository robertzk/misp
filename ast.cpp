#include "ast.h"
#include <iostream>

/*
 * MATOM
 */

struct MATOM_index_does_not_exist : std::exception { char const* what() const throw(); };

int MATOM::count = 0;
std::map<std::string,int> MATOM::map = std::map<std::string,int>();

MATOM::MATOM(int i) {
    this->index = i;
  }

MATOM::MATOM(std::string s) {
  auto it = this->map.find(s);
  if (it == this->map.end()) { // MATOM does not exist yet
    this->map[s] = this->count;
    this->index = this->count;
    this->count += 1;
  } else { // MATOM exists in map. Create singleton?
    this->index = it->second;
  }
}

int MATOM::get_index() const {
  return this->index;
}

std::string MATOM::to_str() const {
  auto it = this->map.begin();
  for (int count = 0; it != this->map.end() && it->second != this->index; it++, count++);

  if (it == this->map.end()) {
    // throw MATOM_index_does_not_exist(); 
    throw std::exception();
  } else {
    return it->first;
  }
}

void MATOM::destroy_symbols() {
  map.clear();
  count = 0;
}

/*
 * MEXPR
 */


MEXPR::MEXPR(MEXP *_parent) {
  this->parent = _parent;
}

bool MEXPR::push_child(MEXP* child) {
  try {
    children.push_back(child);
  } catch(...) {
    return false;
  }
  return true;
}

