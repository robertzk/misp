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
    // TODO: Cache the [ operator
    if (is_square_bracket_expression(node)) {
      make_list(node, sibling, this);     
    } else if (is_brace_bracket_expression(node)) {
      make_dict(node, sibling, this);     
    } else {
      MispBinding::apply(node, sibling);
    }
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
  binding_table["el"]     = &PythonExecutionBinding::element;
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
      static_cast<PythonExecutionBinding*>(binding)->apply(children->at(i), NULL);
      static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
    } else {
      // TODO: Nested non-atomic expression... what to do?
      std::cerr << "Nested non-atomic expression in call_function";
      throw std::exception();
    }
  }
	output << ")";
}

void PythonExecutionBinding::element(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  auto children = node->val.node->get_children();
  // TODO: Fix this hackiness
  if (is_square_bracket_expression(children->at(1))) {
    static_cast<PythonExecutionBinding*>(binding)->apply(children->at(1), NULL);
    static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
  } else {
    output << MEXP_TO_STR(children->at(1));
  }
  output << "[";

  if (children->size() != 3) {
    // TODO: Nested non-atomic expression... what to do?
    std::cerr << "Primitive 'el' called with arity != 2";
    throw std::exception();
  } else if (is_atom(children->at(2))) {
    output << MEXP_TO_STR(children->at(2));
  } else if (is_atomic_expression(children->at(2))) {
    // TODO: Figure out whether it makes sense to pass ->at(i+1) as sibling
    static_cast<PythonExecutionBinding*>(binding)->apply(children->at(2), NULL);
    static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
  } else {
    // TODO: Nested non-atomic expression... what to do?
    std::cerr << "Nested non-atomic expression in primitive 'el'";
    throw std::exception();
  }
	output << "]";
}

void PythonExecutionBinding::print(MEXP* node, MEXP* sibling, MispBinding *binding) {
  // TODO: Check arity
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  output << "print ";
  // TODO: Error if arity > 2
  auto cast_binding = static_cast<PythonExecutionBinding*>(binding);
  auto printed_node = node->val.node->get_children()->at(1);

  if (is_square_bracket_expression(printed_node) ||
      is_brace_bracket_expression(printed_node) ||
      is_atomic_expression(printed_node)) {
    cast_binding->apply(printed_node, NULL);
    cast_binding->increment_skipcount();
  } else if (is_atom(printed_node)) {
    output << MEXP_TO_STR(printed_node);
  } else {
    // TODO: Check if this is actually a nested expression. Should we actually throw?
    std::cerr << "Nested non-atomic expression in print";
    throw std::exception();
  }
	output << ";\n";
}

void PythonExecutionBinding::make_list(MEXP* node, MEXP* sibling, MispBinding *binding) {
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  output << "[";

  auto children = node->val.node->get_children();
  for (unsigned int i = 0; i < children->size(); i++) {
    if (i > 0) {
      output << ", ";
    }
    if (is_atom(children->at(i))) {
      output << MEXP_TO_STR(children->at(i));
    } else if (is_atomic_expression(children->at(i))) {
      // TODO: Figure out whether it makes sense to pass ->at(i+1) as sibling
      static_cast<PythonExecutionBinding*>(binding)->apply(children->at(i), NULL);
      static_cast<PythonExecutionBinding*>(binding)->increment_skipcount();
    } else {
      // TODO: Nested non-atomic expression... what to do?
      std::cerr << "Nested non-atomic expression in make_list";
      throw std::exception();
    }
  }

  output << "]";
}

void PythonExecutionBinding::make_dict(MEXP* node, MEXP* sibling, MispBinding *binding) {
  std::stringstream &output = static_cast<PythonExecutionBinding*>(binding)->get_pyprogram();
  output << "{";

  auto children = node->val.node->get_children();
  for (unsigned int i = 0; i < children->size(); i++) {
    if (i > 0) {
      output << ", ";
    }
    if (is_atom(children->at(i)) || is_atomic_expression(children->at(i))) {
      // TODO: Nested non-atomic expression... what to do?
      std::cerr << "Cannot construct a dictionary using atoms or atomic expressions";
      throw std::exception();
    }

    // TODO: Check children are arity 2
    auto cast_binding = static_cast<PythonExecutionBinding*>(binding);

    // TODO: Handle atoms here too instead of (string foo)
    cast_binding->apply(children->at(i)->val.node->get_children()->at(0), NULL);
    output << ": ";
    cast_binding->apply(children->at(i)->val.node->get_children()->at(1), NULL);

    // TODO: I think this has problems... what if we need to do nested expression evaluation
    // later in the parent expression?
    cast_binding->increment_skipcount();
    cast_binding->increment_skipcount(); 
  }

  output << "}";
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

