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

/*
 * PythonExecutionBinding
 */

void PythonExecutionBinding::apply(MEXP *node, MEXP *sibling) {  
  if (skipcount == 0) {
    MispBinding::apply(node, sibling);
  } else {
    // Do not re-execute sub-trees already executed by a parent tree.
    // TODO: Find a better way to do this.
    skipcount--;
  }
}

void PythonExecutionBinding::initialize_table() {
  binding_table["import"] = &PythonExecutionBinding::import;
  // The dot application operator, e.g., (x subprocess call) == subprocess.call
  binding_table["x"]      = &PythonExecutionBinding::apply_method;
  binding_table["string"] = &PythonExecutionBinding::make_string;
  binding_table["call"]   = &PythonExecutionBinding::call_function;
  binding_table["print"]  = &PythonExecutionBinding::print;
  binding_table["*"]      = &PythonExecutionBinding::unknown_binding;
}

void PythonExecutionBinding::apply_method(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  output <<
    MEXP_TO_STR(node->val.node->get_children()->at(1)) << "." <<
    MEXP_TO_STR(node->val.node->get_children()->at(2)) << "(";
  // TODO: Figure out whether it makes sense to pass ->at(4) as sibling
  static_cast<PythonExecutionBinding*>(binding)->apply(node->val.node->get_children()->at(3), NULL);
	static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
	output << ")";
}

void PythonExecutionBinding::call_function(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  auto children = node->val.node->get_children();
  output << MEXP_TO_STR(children->at(1)) << "(";
  for (unsigned int i = 2; i < children->size(); i++) {
    if (i > 2) {
      output << ", ";
    }
    if (is_atom(children->at(i))) {
      output << MEXP_TO_STR(children->at(i));
    } else if (is_atomic_expression(children->at(i))) {
      // TODO: Figure out whether it makes sense to pass ->at(i+1) as sibling
      static_cast<PythonExecutionBinding*>(binding)->apply(node->val.node->get_children()->at(i), NULL);
      static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
    } else {
      // TODO: Nested non-atomic expression... what to do?
      std::cerr << "Nested non-atomic expression in call_function";
      throw std::exception();
    }
  }
	output << ")";
}

void PythonExecutionBinding::print(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  output << "print ";
  // TODO: Error if arity > 2
  if (is_atomic_expression(node->val.node->get_children()->at(1))) {
    static_cast<PythonExecutionBinding*>(binding)->apply(node->val.node->get_children()->at(1), NULL);
    static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
  } else {
    output << MEXP_TO_STR(node->val.node->get_children()->at(1));
  }
	output << ";\n";
}

void PythonExecutionBinding::make_string(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  static_cast<PythonExecutionBinding*>(binding)->get_pyprogram() <<
    '"' << MEXP_TO_STR(node->val.node->get_children()->at(1)) << '"';
}

void PythonExecutionBinding::import(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  static_cast<PythonExecutionBinding*>(binding)->get_pyprogram() <<
    "import " << MEXP_TO_STR(node->val.node->get_children()->at(1)) << ";\n";
}

void PythonExecutionBinding::unknown_binding(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO Handle exception correctly
  std::cerr << "No python binding for atom '" << MEXP_TO_STR(node->val.node->get_children()->at(0)) <<
    "' in expression `" << MEXP_TO_STR(node) << "`'\n";
  throw std::exception();
}

/*
 * PythonExecutionStrategy
 */

void PythonExecutionStrategy::execute() {
  ApplyBindingLTRStrategy::execute();

  this->pystream << static_cast<PythonExecutionBinding*>(this->binding)->get_pyprogram().str();
}

void PythonExecutionStrategy::effect(MEXP *node, MEXP *sibling) {
  if (is_atomic_expression(node)) {
    // We can't call ApplyBindingLTRStrategy::effect(node, sibling)
    // directly, as we will then be calling the wrong apply! Lots of
    // tricky polymorphism going on here... This makes the code less DRY
    // but I don't know how else to do this at the moment.
    static_cast<PythonExecutionBinding*>(this->binding)->apply(node, sibling);
  }
}

void PythonExecutionStrategy::finalize(MEXP *node, MEXP *sibling) {

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

