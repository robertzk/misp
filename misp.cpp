#include "misp.h"

// For temp files: https://en.cppreference.com/w/cpp/io/c/tmpfile
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>

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
  } else if (svector_contains(flags, "--shell")) {
    MEXP *code = (new MexprBuilder(misp_source))->parse();
    ShellExecutionStrategy(code).execute();
  } else if (svector_contains(flags, "--python")) {
    MEXP *code = (new MexprBuilder(misp_source))->parse();
    std::string tempfilename = std::tmpnam(nullptr);
    std::ofstream tempfile = std::ofstream(tempfilename);

    // TODO: Add --debug interaction: std::cout << "Executing into: " << tempfilename << "\n";
    //PythonExecutionStrategy(code, std::cout, std::cout, tempfilename).execute();
    PythonExecutionStrategy(code, std::cout, tempfile, tempfilename).execute();
    tempfile.close();

    // TODO: Add --debug interaction: std::cout << "Executing cmd: " << std::string("python ") + tempfilename << "\n";
    std::cout << ShellExecutionBinding::shell_exec(std::string("python ") + tempfilename);
  } else if (svector_contains(flags, "--debug")) {
    MEXP *code = (new MexprBuilder(misp_source))->parse();
    DebugTokensStrategy(code).execute();
  } else { // if (svector_contains(flags, "--onto=print")) {
    MEXP *code = (new MexprBuilder(misp_source))->parse();
    ApplyBindingLTRStrategy(code).execute();
  }
}

