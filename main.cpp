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
};

int MATOM::count = 0;
std::map<std::string,int> MATOM::map = std::map<std::string,int>();

class MEXPR {

};

struct MATOM_index_does_not_exist : std::exception { char const* what() const throw(); };

MATOM::MATOM(int i) {
    this->index = i;
  }

MATOM::MATOM(std::string s) {
  auto it = this->map.find(s);
  if (it == this->map.end()) { // MATOM does not exist yet
    this->map[s] = this->count;
    this->count += 1;
    MATOM(this->count - 1);
  } else { // MATOM exists in map. Create singleton?
    MATOM(it->second);
  }
}

int MATOM::get_index() const {
  return this->index;
}

std::string MATOM::to_str() const {
  auto it = this->map.begin();
  for (int count = 0; it != this->map.end() && count != index; it++, count++);

  if (it == this->map.end()) {
    // throw MATOM_index_does_not_exist(); 
    throw std::exception();
  } else {
    return it->first;
  }
}

#include <iostream>

int main(int argc, char** argv) {
  MATOM *m = new MATOM("foo");

  std::cout << "MISP! " << m->to_str();

  return 0;
}

