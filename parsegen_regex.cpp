#include "parsegen_regex.hpp"

#include <iostream>
#include <sstream>

#include "parsegen_build_parser.hpp"
#include "parsegen_chartab.hpp"
#include "parsegen_reader.hpp"
#include "parsegen_set.hpp"
#include "parsegen_std_vector.hpp"
#include "parsegen_string.hpp"

namespace parsegen {
namespace regex {

language build_language() {
  /* The top produtions were from the "grep.y" YACC grammar in the source
     code for Plan 9's grep utility, see here:
https://github.com/wangeguo/plan9/blob/master/sys/src/cmd/grep/grep.y
     The "set" related productions
     are from a grammar intended to be used by ProLog to parse Perl's regular
     expressions, see here:
http://www.cs.sfu.ca/~cameron/Teaching/384/99-3/regexp-plg.html */
  language out;
  auto& prods = out.productions;
  prods.resize(NPRODS);
  prods[PROD_REGEX] = {"regex", {"union"}};
  prods[PROD_UNION_DECAY] = {"union", {"concat"}};
  prods[PROD_UNION] = {"union", {"union", "|", "concat"}};  // union
  prods[PROD_CONCAT_DECAY] = {"concat", {"qualified"}};
  prods[PROD_CONCAT] = {"concat", {"concat", "qualified"}};  // concatenation
  prods[PROD_QUAL_DECAY] = {"qualified", {"single"}};
  prods[PROD_STAR] = {"qualified", {"qualified", "*"}};
  prods[PROD_PLUS] = {"qualified", {"qualified", "+"}};
  prods[PROD_MAYBE] = {"qualified", {"qualified", "?"}};
  prods[PROD_SINGLE_CHAR] = {"single", {"char"}};
  prods[PROD_ANY] = {"single", {"."}};  // any
  prods[PROD_SINGLE_SET] = {"single", {"set"}};
  prods[PROD_PARENS_UNION] = {"single", {"(", "union", ")"}};
  prods[PROD_SET_POSITIVE] = {"set", {"positive-set"}};
  prods[PROD_SET_NEGATIVE] = {"set", {"negative-set"}};
  prods[PROD_POSITIVE_SET] = {"positive-set", {"[", "set-items", "]"}};
  prods[PROD_NEGATIVE_SET] = {"negative-set", {"[", "^", "set-items", "]"}};
  prods[PROD_SET_ITEMS_DECAY] = {"set-items", {"set-item"}};
  prods[PROD_SET_ITEMS_ADD] = {"set-items", {"set-items", "set-item"}};
  prods[PROD_SET_ITEM_CHAR] = {"set-item", {"char"}};
  prods[PROD_SET_ITEM_RANGE] = {"set-item", {"range"}};
  prods[PROD_RANGE] = {"range", {"char", "-", "char"}};
  out.tokens.resize(NTOKS);
  /* either one of the non-meta characters, or anything preceded by the escape
   * slash */
  out.tokens[TOK_CHAR] = {
      "char", "[^\\\\\\.\\[\\]\\(\\)\\|\\-\\^\\*\\+\\?]|\\\\."};
  out.tokens[TOK_DOT] = {".", "\\."};
  out.tokens[TOK_LRANGE] = {"[", "\\]"};
  out.tokens[TOK_RRANGE] = {"]", "\\]"};
  out.tokens[TOK_LPAREN] = {"(", "\\("};
  out.tokens[TOK_RPAREN] = {")", "\\)"};
  out.tokens[TOK_UNION] = {"|", "\\|"};
  out.tokens[TOK_RANGE] = {"-", "\\-"};
  out.tokens[TOK_NEGATE] = {"^", "\\^"};
  out.tokens[TOK_STAR] = {"*", "\\*"};
  out.tokens[TOK_PLUS] = {"+", "\\+"};
  out.tokens[TOK_MAYBE] = {"?", "\\?"};
  return out;
}

/* bootstrap ! This lexer is used to build the reader_tables that read
   regular expressions themselves, so it can't depend on that reader ! */
finite_automaton build_lexer() {
  std::string meta_chars_str = ".[]()|-^*+?";
  std::set<int> all_chars;
  for (int i = 0; i < NCHARS; ++i) all_chars.insert(i);
  auto nonmeta_chars = all_chars;
  for (auto meta_char : meta_chars_str) {
    auto it = nonmeta_chars.find(get_symbol(meta_char));
    nonmeta_chars.erase(it);
  }
  auto lex_nonmeta =
      finite_automaton::make_set_nfa(NCHARS, nonmeta_chars, TOK_CHAR);
  auto lex_slash = make_char_single_nfa('\\');
  auto lex_any = finite_automaton::make_set_nfa(NCHARS, all_chars);
  auto lex_escaped = finite_automaton::concat(lex_slash, lex_any, TOK_CHAR);
  auto lex_char = finite_automaton::unite(lex_nonmeta, lex_escaped);
  finite_automaton lex_metachars;
  for (int i = 0; i < size(meta_chars_str); ++i) {
    int token = TOK_CHAR + i + 1;
    auto lex_metachar = make_char_single_nfa(at(meta_chars_str, i), token);
    if (i)
      lex_metachars = finite_automaton::unite(lex_metachars, lex_metachar);
    else
      lex_metachars = lex_metachar;
  }
  auto out = finite_automaton::unite(lex_char, lex_metachars);
  return finite_automaton::simplify(finite_automaton::make_deterministic(out));
}

reader_tablesPtr ask_reader_tables() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
  static reader_tablesPtr ptr;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  if (ptr.use_count() == 0) {
    auto lang = regex::ask_language();
    auto grammar = build_grammar(*lang);
    auto parser = accept_parser(build_lalr1_parser(grammar));
    auto lexer = regex::build_lexer();
    indentation indent_info;
    indent_info.is_sensitive = false;
    indent_info.indent_token = -1;
    indent_info.dedent_token = -1;
    ptr.reset(new reader_tables{parser, lexer, indent_info});
  }
  return ptr;
}

languagePtr ask_language() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
  static languagePtr ptr;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  if (ptr.use_count() == 0) {
    ptr.reset(new language(build_language()));
  }
  return ptr;
}

finite_automaton build_dfa(
    std::string const& name, std::string const& regex, int token) {
  auto reader = regex::reader(token);
  try {
    return std::any_cast<finite_automaton>(reader.read_string(regex, name));
  } catch (const parse_error& e) {
    std::stringstream ss;
    ss << e.what() << '\n';
    ss << "error: couldn't build DFA for token \"" << name << "\" regex \""
       << regex << "\"\n";
    ss << "repeating with debug_reader:\n";
    debug_reader debug_reader(regex::ask_reader_tables(), ss);
    debug_reader.read_string(regex, name);
    throw parse_error(ss.str());
  }
}

regex::reader::reader(int result_token_in)
    : parsegen::reader(regex::ask_reader_tables()),
      result_token(result_token_in) {}

std::any regex::reader::at_shift(int token, std::string& text) {
  if (token != TOK_CHAR) {
    return std::any();
  }
  if (size(text) == 1) {
    return std::any(text[0]);
  } else if (size(text) == 2) {
    assert(text[0] == '\\');
    return std::any(text[1]);
  } else {
    std::cerr << "BUG: regex char text is \"" << text << "\"\n";
    abort();
  }
}

std::any regex::reader::at_reduce(int production, std::vector<std::any>& rhs) {
  switch (production) {
    case PROD_REGEX:
      return finite_automaton::simplify(finite_automaton::make_deterministic(
          std::any_cast<finite_automaton&&>(std::move(at(rhs, 0)))));
    case PROD_UNION_DECAY:
    case PROD_CONCAT_DECAY:
    case PROD_QUAL_DECAY:
    case PROD_SET_ITEMS_DECAY:
    case PROD_SET_ITEM_RANGE:
      return at(rhs, 0);
    case PROD_UNION:
      return finite_automaton::unite(std::any_cast<finite_automaton&&>(std::move(at(rhs, 0))),
          std::any_cast<finite_automaton&&>(std::move(at(rhs, 2))));
    case PROD_CONCAT: {
      auto& a_any = at(rhs, 0);
      auto& b_any = at(rhs, 1);
      auto a = std::any_cast<finite_automaton&&>(std::move(a_any));
      auto b = std::any_cast<finite_automaton&&>(std::move(b_any));
      return finite_automaton::concat(a, b, result_token);
    }
    case PROD_STAR:
      return finite_automaton::star(
          std::any_cast<finite_automaton&&>(std::move(at(rhs, 0))), result_token);
    case PROD_PLUS:
      return finite_automaton::plus(
          std::any_cast<finite_automaton&&>(std::move(at(rhs, 0))), result_token);
    case PROD_MAYBE:
      return finite_automaton::maybe(
          std::any_cast<finite_automaton&&>(std::move(at(rhs, 0))), result_token);
    case PROD_SINGLE_CHAR:
      return make_char_single_nfa(std::any_cast<char>(at(rhs, 0)), result_token);
    case PROD_ANY:
      return finite_automaton::make_range_nfa(
          NCHARS, 0, NCHARS - 1, result_token);
    case PROD_SINGLE_SET:
      return make_char_set_nfa(
          std::any_cast<std::set<char>&&>(std::move(at(rhs, 0))), result_token);
    case PROD_PARENS_UNION:
      return at(rhs, 1);
    case PROD_SET_POSITIVE:
      return at(rhs, 0);
    case PROD_SET_NEGATIVE:
      return negate_set(std::any_cast<std::set<char>&&>(std::move(at(rhs, 0))));
    case PROD_POSITIVE_SET:
      return at(rhs, 1);
    case PROD_NEGATIVE_SET:
      return at(rhs, 2);
    case PROD_SET_ITEMS_ADD:
      return unite(std::any_cast<std::set<char>&&>(std::move(at(rhs, 0))),
          std::any_cast<std::set<char>&&>(std::move(at(rhs, 1))));
    case PROD_SET_ITEM_CHAR:
      return std::set<char>({std::any_cast<char>(at(rhs, 0))});
    case PROD_RANGE: {
      std::set<char> set;
      for (char c = std::any_cast<char>(at(rhs, 0));
           c <= std::any_cast<char>(at(rhs, 2)); ++c) {
        set.insert(c);
      }
      return std::any(std::move(set));
    }
  }
  std::cerr << "BUG: unexpected production " << production << '\n';
  abort();
}

}  // end namespace regex
}  // end namespace parsegen
