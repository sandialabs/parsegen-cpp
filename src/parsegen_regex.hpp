#ifndef PARSEGEN_REGEX_HPP
#define PARSEGEN_REGEX_HPP

#include "parsegen_finite_automaton.hpp"
#include "parsegen_language.hpp"
#include "parsegen_parser.hpp"
#include "parsegen_parser_tables.hpp"

namespace parsegen {
namespace regex {

enum {
  PROD_REGEX,
  PROD_UNION_DECAY,
  PROD_UNION,
  PROD_CONCAT_DECAY,
  PROD_CONCAT,
  PROD_QUAL_DECAY,
  PROD_STAR,
  PROD_PLUS,
  PROD_MAYBE,
  PROD_SINGLE_CHAR,
  PROD_ANY,
  PROD_SINGLE_SET,
  PROD_PARENS_UNION,
  PROD_SET_POSITIVE,
  PROD_SET_NEGATIVE,
  PROD_POSITIVE_SET,
  PROD_NEGATIVE_SET,
  PROD_SET_ITEMS_DECAY,
  PROD_SET_ITEMS_ADD,
  PROD_SET_ITEM_CHAR,
  PROD_SET_ITEM_RANGE,
  PROD_RANGE,
};

enum { NPRODS = PROD_RANGE + 1 };

enum {
  TOK_CHAR,
  TOK_DOT,
  TOK_LRANGE,
  TOK_RRANGE,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_UNION,
  TOK_RANGE,
  TOK_NEGATE,
  TOK_STAR,
  TOK_PLUS,
  TOK_MAYBE,
};

enum { NTOKS = TOK_MAYBE + 1 };

language build_language();
language_ptr ask_language();

finite_automaton build_lexer();

parser_tables_ptr ask_parser_tables();

finite_automaton build_dfa(
    std::string const& name, std::string const& regex, int token);

std::any shift_internal(int token, std::string& text);
std::any reduce_internal(int production, std::vector<std::any>& rhs, int result_token);

class parser : public parsegen::parser {
 public:
  parser(int result_token_in);
  parser(parser const&) = default;
  virtual ~parser() override = default;

 protected:
  virtual std::any shift(int token, std::string& text) override;
  virtual std::any reduce(int token, std::vector<std::any>& rhs) override;

 private:
  int result_token;
};

bool matches(std::string const& r, std::string const& t);

std::string from_charset(std::set<char> const& s);
std::string from_automaton(finite_automaton const& fa);

std::string for_first_occurrence_of(std::string const& s);
std::string for_case_insensitive(std::string const& s);

std::string maybe_sign();
std::string leading_digits();
std::string trailing_digits();
std::string unsigned_floating_point_not_integer();
std::string unsigned_integer();
std::string unsigned_floating_point();
std::string signed_integer();
std::string signed_floating_point_not_integer();
std::string signed_floating_point();
std::string whitespace();
std::string identifier();
std::string C_style_comment();

}  // end namespace regex

inline std::string anycase(std::string const& a)
{
  return parsegen::regex::for_case_insensitive(a);
}

}  // end namespace parsegen

#endif
