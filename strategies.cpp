#include "strategies.h"

// For shell execution: https://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <sstream>

/*
 * ShellExecutionBinding
 */


void ShellExecutionBinding::initialize_table() {
  binding_table["*"] = &ShellExecutionBinding::exec;
}

bool ShellExecutionBinding::is_encloser_atom(MEXP *node) {
  return (MEXP_IS_ATOM(node) && node->val.atom->encloser());
}

bool ShellExecutionBinding::is_symbolic_atom(MEXP *node) {
  return (MEXP_IS_ATOM(node) && !(node->val.atom->encloser()));
}

bool ShellExecutionBinding::is_shell_expression(MEXP *node) {
  return (MEXP_IS_EXPR(node) &&
    ShellExecutionBinding::is_encloser_atom(node->val.node->get_parent()) &&
    ShellExecutionBinding::is_symbolic_atom(node->val.node->get_children()->at(0)));
}

void ShellExecutionBinding::exec(MEXP* node, MEXP* sibling, MispBinding *binding) {
  if (ShellExecutionBinding::is_shell_expression(node)) {
    std::stringstream ss;
    bool first = true;
    for (auto child : *(node->val.node->get_children())) {
      ss << (first ? "" : " ") << MEXP_TO_STR(child);
      first = false;
    }
    (static_cast<ShellExecutionBinding*>(binding))->get_os() <<
      ShellExecutionBinding::shell_exec(ss.str());
  }
}

std::string ShellExecutionBinding::shell_exec(std::string cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
			result += buffer.data();
		}
	}
	return result;
}

/*
 * ShellExecutionStrategy
 */


void ShellExecutionStrategy::finalize(MEXP *node, MEXP *sibling) {

}

