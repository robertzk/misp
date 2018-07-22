#include "tokenizer.h"

bool MexprBuilder::is_whitespace(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

MEXP* MexprBuilder::parse(bool default_parentheses) {
  std::vector<std::string> tokens = tokenize(default_parentheses);
  unsigned int offset = 0;

  if (tokens.size() == 0) {
    return NULL;
  }

  if (!MATOM::is_encloser_char(tokens[0][0])) {
    // TODO right exception
    std::cerr << "Expected encloser token, got " << tokens[0] << std::endl;
    throw std::exception();
  }

  return parse_list(tokens, offset);
}

MEXP* MexprBuilder::parse_list(std::vector<std::string> tokens, unsigned int &offset) {
  if (!MATOM::is_encloser_body_char(tokens[offset][0])) {
    return new MEXP { { .atom = new MATOM(tokens[offset++]) }, MEXP_TYPE::matom_type };
  }

  if (MATOM::is_closer_char(tokens[offset][0])) {
    // TODO: Unexpected closer! Throw right exception.
    throw std::exception();
  }

  MEXP* encloser = new MEXP { { .atom = new MATOM(tokens[offset]) }, MEXP_TYPE::matom_type };
  MEXP* list  = new MEXP { { .node = new MEXPR(encloser) }, MEXP_TYPE::mexpr_type };
  
  std::string expected_closer = MATOM::generate_closer(tokens[offset]);

  offset += 1;
  while (offset < tokens.size() && tokens[offset] != expected_closer) {
    if (MATOM::is_closer_char(tokens[offset][0])) {
      // Should never happen! Invalid closer. TODO: Exception.
      throw std::exception();
    }
    MEXP *element = this->parse_list(tokens, offset);
    list->val.node->push_child(element);
  }

  if (tokens[offset] == expected_closer) {
    offset += 1;
  }

  if (false && offset < tokens.size()) { // TODO: Figure this out.
    // Premature closer!
    std::cerr << "Premature closer. Not sure what to do with rest of token sequence.\n";
    throw std::exception();
  }

  return list;
}

std::vector<std::string> MexprBuilder::tokenize(bool default_parentheses) {
  std::vector<std::string> tokens;
  std::stack<std::string> expected_closer;

  if (default_parentheses) {
    tokens.push_back("(");
  }

  unsigned int i = 0;
  for (i = 0; i < source.size(); i++) {
    char ch = source[i];
    if (is_whitespace(ch)) { continue; }
    
    std::string token;

    if (MATOM::is_encloser_body_char(ch)) {
      bool is_closer = MATOM::is_closer_char(ch);

      while (MATOM::is_encloser_body_char(ch)) {
        token.push_back(ch);
        i++;
        if (i >= source.size()) { break; }
        ch = source[i];
      }
      
      if (is_closer) {
        if (token != expected_closer.top()) {
          std::cerr << "Expected closer " << expected_closer.top() << ", instead got: " << token << std::endl;
          throw std::exception();
        } else {
          expected_closer.pop();
        }
      } else {
        expected_closer.push(MATOM::generate_closer(token));
      }
      tokens.push_back(token);
      i--; // Go back a step so we don't skip a character.
      token.clear();
      continue;
    } // end encloser char check

    // Atom character or literal
    while (!is_whitespace(ch) && !MATOM::is_encloser_body_char(ch)) {
      token.push_back(ch);
      i++;
      ch = source[i];
    }

    tokens.push_back(token);

    i--; // Go back a step so we don't skip a character.
    token.clear();
  }

  if (default_parentheses) {
    tokens.push_back(")");
  }

  return tokens;
} 

