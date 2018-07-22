#include "execution.h"

/*
 * LTRTraverseStrategy
 */

void LTRTraverseStrategy::execute1(MEXP *node, MEXP *sibling) {
  if (MEXP_IS_ATOM(node)) {
    effect(node, sibling);
  } else {
    // Parent -> children tree walk
    execute1(node->val.node->get_parent());
    auto children = node->val.node->get_children();
    for (unsigned int i = 0; i < children->size(); i++) {
      auto child = children->at(i);
      auto sibling_node = child;
      if (i == children->size() - 1) {
        sibling_node = NULL;
      } else {
        sibling_node = children->at(i + 1);
      }
      // TODO: Should we pass the full tail here? Or maybe just let do a SIBLING macro?
      execute1(child, sibling_node);
    }
    finalize(node, sibling);
  }
}

void LTRTraverseStrategy::execute() {
  execute1(this->program);
}

/*
 * LTRAtomPrinterStrategy
 */

void LTRAtomPrinterStrategy::execute() {
  os << "Executing: \n";
  LTRTraverseStrategy::execute();
  os << "\nExecution completed.\n";
}

void LTRAtomPrinterStrategy::effect(MEXP *node, MEXP *sibling) {
  if (MEXP_IS_ATOM(node)) {
    os << "Atom: " << MEXP_TO_STR(node) << std::endl;
  }
}

void LTRAtomPrinterStrategy::finalize(MEXP *node, MEXP *sibling) {
  if (MEXP_IS_EXPR(node)) {
    os << "Finalizing: " << MEXP_TO_STR(node->val.node->get_parent()) << std::endl;
  }
}

/* 
 * ApplyBindingLTRStrategy
 */

ApplyBindingLTRStrategy::ApplyBindingLTRStrategy(MEXP *_program, MispBinding *_binding) : LTRTraverseStrategy(_program){
  if (_binding == NULL) {
    this->binding = new PrintBinding();
  } else {
    this->binding = _binding;
  }
}

void ApplyBindingLTRStrategy::effect(MEXP *node, MEXP *sibling) {
  if (MEXP_IS_ATOM(node)) {
    this->binding->apply(node, sibling);
  }
}

void ApplyBindingLTRStrategy::finalize(MEXP *node, MEXP *sibling) {
   // No op.
}

/*
 * MispBinding
 */

void MispBinding::apply(MEXP *node, MEXP *sibling) {
  if (MEXP_IS_ATOM(node)) {
    auto it = this->binding_table.find(node->val.atom->to_str());
    if (it != this->binding_table.end()) { // Bound lambda
      (*it->second)(node, sibling, this);
    } else {
      // TODO: Cache * route
      it = this->binding_table.find("*");
      if (it != this->binding_table.end()) { // Catch-all binding
        (*it->second)(node, sibling, this);
      }
    }
  }
}

/*
 * PrintBinding
 */

void PrintBinding::initialize_table() {
  binding_table["print"] = &PrintBinding::print;
}

void PrintBinding::print(MEXP* node, MEXP* sibling, MispBinding *binding) {
  if (MEXP_IS_ATOM(node) && node->val.atom->to_str() == "print") {
    if (sibling != NULL) {
      (static_cast<PrintBinding*>(binding))->get_os() << MEXP_TO_STR(sibling);
    }
  }
}

/*
 * TokenDebugBinding
 */

void TokenDebugBinding::initialize_table() {
  binding_table["*"] = &TokenDebugBinding::print_token;
}

void TokenDebugBinding::print_token(MEXP* node, MEXP* sibling, MispBinding *binding) {
  if (MEXP_IS_ATOM(node)) {
    (static_cast<TokenDebugBinding*>(binding))->get_os() <<
      "Token: " << MEXP_TO_STR(node) << std::endl;
  }
}

/* 
 * DebugTokensStrategy
 */

void DebugTokensStrategy::finalize(MEXP *node, MEXP *sibling) {
  (static_cast<TokenDebugBinding*>(binding))->get_os() <<
    "Finalizing: " << MATOM::generate_closer(MEXP_TO_STR(node->val.node->get_parent())) << std::endl;
}


