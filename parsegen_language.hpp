#ifndef PARSEGEN_LANGUAGE_HPP
#define PARSEGEN_LANGUAGE_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "parsegen_finite_automaton.hpp"
#include "parsegen_grammar.hpp"
#include "parsegen_reader_tables.hpp"

namespace parsegen {

struct language {
  struct Token {
    std::string name;
    std::string regex;
  };
  std::vector<Token> tokens;
  struct Production {
    std::string lhs;
    std::vector<std::string> rhs;
  };
  std::vector<Production> productions;
};

using languagePtr = std::shared_ptr<language>;

grammarPtr build_grammar(language const& language);

finite_automaton build_lexer(language const& language);

reader_tablesPtr build_reader_tables(language const& language);

std::ostream& operator<<(std::ostream& os, language const& lang);

}  // namespace parsegen

#endif
