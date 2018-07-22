#include "tokenizer.h"

bool MexprBuilder::is_whitespace(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

MEXP* MexprBuilder::parse(bool default_parentheses) {
  std::vector<std::string> tokens = tokenize(default_parentheses);

  return new MEXP { { .atom = new MATOM(tokens[0]) }, MEXP_TYPE::matom_type };
}

std::vector<std::string> MexprBuilder::tokenize(bool default_parentheses) {
  std::vector<std::string> tokens;
  if (default_parentheses) {
    tokens.push_back("(");
  }

  std::stack<std::string> expected_closer;

  unsigned int i = 0;
  for (i = 0; i < source.size(); i++) {
    char ch = source[i];
    if (is_whitespace(ch)) { continue; }
    
    std::string token;

    if (MATOM::is_encloser_char(ch)) {
      bool is_closer = MATOM::is_closer_char(ch);

      while (MATOM::is_encloser_body_char(ch)) {
        token.push_back(ch);
        i++;
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

