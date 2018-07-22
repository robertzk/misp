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

void ApplyBindingLTRStrategy::effect(MEXP *node, MEXP *sibling) {
  if (MEXP_IS_ATOM(node)) {
    this->binding->apply(node, sibling);
  }
}

void ApplyBindingLTRStrategy::finalize(MEXP *node, MEXP *sibling) {

}


