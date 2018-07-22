#include "ast.h"

/*
 * MATOM
 */

struct MATOM_index_does_not_exist : std::exception { char const* what() const throw(); };
struct MATOM_not_an_encloser : std::exception { char const* what() const throw(); };

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

  this->is_encloser = is_encloser_string(s);
  if (this->is_encloser) {
    this->closer = generate_closer(s);
  }
}

int MATOM::get_index() const {
  return this->index;
}

std::string MATOM::get_closer() const {
  if (this->is_encloser) {
    return this->closer;
  } else {
    // throw MATOM_not_an_encloser();
    throw std::exception();
  }
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

bool MATOM::is_encloser_body_char(char ch) {
  static char encloser_body_chars[31] = "`~!@#$%^&*()_+-={}[]|\\:;?/.,<>";
  for (unsigned int i = 0; i < 31; i++) {
    if (ch == encloser_body_chars[i]) {
      return true;
    }
  }
  return false;
}

bool MATOM::is_encloser_char(char ch) {
  return ch == '(' || ch == '[' || ch == '{' || ch == '<';
}

bool MATOM::is_closer_char(char ch) {
  return ch == ')' || ch == ']' || ch == '}' || ch == '>';
}

bool MATOM::is_encloser_string(std::string s) {
  // TODO: Exclude strings like "(foo"
  for (auto ch : s) {
    if (is_encloser_char(ch)) {
      return true;
    }
  }
  return false;
}

bool MATOM::is_closer_string(std::string s) {
  // TODO: Exclude strings like "foo)"
  for (auto ch : s) {
    if (is_closer_char(ch)) {
      return true;
    }
  }
  return false;
}

std::string MATOM::generate_closer(std::string s) {
  std::string c;
  if (s.size() == 0) {
    return c;
  }
  for (unsigned int i = s.size(); i > 0; i--) {
    switch(s[i - 1]) {
      case '(': c.push_back(')'); break;
      case '[': c.push_back(']'); break;
      case '{': c.push_back('}'); break;
      case '<': c.push_back('>'); break;
      default: c.push_back(s[i - 1]);
    }
  }
  return c;
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

std::string MEXPR::to_str() const {
  std::stringstream ss;

  bool simple_paren = false;
  if (MEXP_IS_ATOM(parent)) {
    if (parent->val.atom->encloser()) {
      std::string encloser_string = MEXP_TO_STR(parent);
      simple_paren = encloser_string.size() == 1;
      ss << encloser_string;
    } else {
      ss << '(' << MEXP_TO_STR(parent);
    }
  }

  bool first = true;
  for (auto child : this->children) {
    ss << (first && simple_paren ? "" : " ") << MEXP_TO_STR(child);
    first = false;
  }

  if (MEXP_IS_ATOM(parent)) {
    if (parent->val.atom->encloser()) {
      if (!simple_paren) {
        ss << ' ';
      }
      ss << parent->val.atom->get_closer();
    }
  }

  return ss.str();
}

