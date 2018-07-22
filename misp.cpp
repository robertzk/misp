#include "misp.h"

int main(int argc, char** argv) {
  process_flags(params_to_vector(argc, argv));

  return 0;
}

std::vector<std::string> params_to_vector(int argc, char** argv) {
  std::vector<std::string> flags;
  for (unsigned int i = 0; i < argc; i++) {
    flags.push_back(std::string(argv[i]));
  }
  return flags;
}

void process_flags(std::vector<std::string> flags) {
  if (flags.size() < 2) {
    std::cout << "Usage: misp <filename>\n";
    exit(1);
  }

  // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  std::ifstream file(flags[flags.size() - 1]);
  std::string misp_source((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

  if (svector_contains(flags, std::string("--parse"))) {
    char sep[26] = "-------------------------";
    std::cout << "MISP input:\n" << sep << "\n" << misp_source << sep << "\n\n";
    MEXP *code = (new MexprBuilder(misp_source))->parse();
    std::cout << "MISP code parsed: \n" << sep << "\n" << MEXP_TO_STR(code) << "\n" << sep << "\n";
  } else { // if (svector_contains(flags, "--onto=print")) {
    MEXP *code = (new MexprBuilder(misp_source))->parse();
    ApplyBindingLTRStrategy(code).execute();
  }
}

