#ifndef OMEGA_H_LANGUAGE_HPP
#define OMEGA_H_LANGUAGE_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "parsegen_finite_automaton.hpp"
#include "parsegen_grammar.hpp"
#include "parsegen_reader_tables.hpp"

namespace parsegen {

struct Language {
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

using LanguagePtr = std::shared_ptr<Language>;

GrammarPtr build_grammar(Language const& language);

FiniteAutomaton build_lexer(Language const& language);

ReaderTablesPtr build_reader_tables(Language const& language);

std::ostream& operator<<(std::ostream& os, Language const& lang);

}  // namespace parsegen

#endif
