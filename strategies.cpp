#include "strategies.h"

// For shell execution: https://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <fstream>
#include <sstream>

/*
 * Helper functions
 */

bool is_atom(MEXP *node) {
  return MEXP_IS_ATOM(node);
}

bool is_square_bracket_expression(MEXP *node) {
  return MEXP_IS_EXPR(node) && is_atom(node->val.node->get_parent()) &&
         MEXP_TO_STR(node->val.node->get_parent()) == "[";
}

bool is_brace_bracket_expression(MEXP *node) {
  return MEXP_IS_EXPR(node) && is_atom(node->val.node->get_parent()) &&
         MEXP_TO_STR(node->val.node->get_parent()) == "{";
}

bool is_encloser_atom(MEXP *node) {
  return (is_atom(node) && node->val.atom->encloser());
}

bool is_symbolic_atom(MEXP *node) {
  return (is_atom(node) && !(node->val.atom->encloser()));
}

bool is_atomic_expression(MEXP *node) {
  return (MEXP_IS_EXPR(node) &&
    is_encloser_atom(node->val.node->get_parent()) &&
    is_symbolic_atom(node->val.node->get_children()->at(0)));
}

/*
 * ShellExecutionBinding
 */


void ShellExecutionBinding::initialize_table() {
  binding_table["*"] = &ShellExecutionBinding::exec;
}

void ShellExecutionBinding::exec(MEXP* node, MEXP* sibling, MispBinding *binding) {
  if (is_atomic_expression(node)) {
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
  if (!pipe) {
  	throw std::runtime_error("popen() failed!");
  }
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


/* 
 * DebugTokensStrategy
 */

void DebugTokensStrategy::finalize(MEXP *node, MEXP *sibling) {
  (static_cast<TokenDebugBinding*>(binding))->get_os() <<
    "Finalizing: " << MATOM::generate_closer(MEXP_TO_STR(node->val.node->get_parent())) << std::endl;
}

void DebugTokensStrategy::effect(MEXP *node, MEXP *sibling) {
  // TODO: Figure out if to use a param for leaf-only walks, or
  // whether to support effects for non-atomic expressions...
  if (is_atom(node) || is_atomic_expression(node)) {
    static_cast<TokenDebugBinding*>(this->binding)->apply(node, sibling);
  }
}

