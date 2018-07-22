#include "misp.h"
#include <string>
#include <fstream>
#include <streambuf>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: misp <filename>\n";
    return 0;
  }

  // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  std::ifstream file(argv[1]);
  std::string misp_source((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());


  std::cout << "MISP input: " << misp_source << "\n";
  MEXP *code = (new MexprBuilder(misp_source))->parse(false);
  std::cout << "MISP code parsed: \n" << MEXP_TO_STR(code) << "\n";

  return 0;
}

