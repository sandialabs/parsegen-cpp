#ifndef PARSEGEN_MATH_LANG_HPP
#define PARSEGEN_MATH_LANG_HPP

#include "parsegen_language.hpp"
#include "parsegen_parser_tables.hpp"
#include <set>

namespace parsegen {

namespace math_lang {

enum {
  PROD_PROGRAM,
  PROD_NO_STATEMENTS,
  PROD_NEXT_STATEMENT,
  PROD_ASSIGN,
  PROD_NO_EXPR,
  PROD_YES_EXPR,
  PROD_EXPR,
  PROD_TERNARY_DECAY,
  PROD_OR_DECAY,
  PROD_AND_DECAY,
  PROD_ADD_SUB_DECAY,
  PROD_MUL_DIV_DECAY,
  PROD_NEG_DECAY,
  PROD_POW_DECAY,
  PROD_TERNARY,
  PROD_OR,
  PROD_AND,
  PROD_GT,
  PROD_LT,
  PROD_GEQ,
  PROD_LEQ,
  PROD_EQ,
  PROD_BOOL_PARENS,
  PROD_ADD,
  PROD_SUB,
  PROD_MUL,
  PROD_DIV,
  PROD_POW,
  PROD_CALL,
  PROD_NO_ARGS,
  PROD_SOME_ARGS,
  PROD_FIRST_ARG,
  PROD_NEXT_ARG,
  PROD_NEG,
  PROD_VAL_PARENS,
  PROD_CONST,
  PROD_VAR,
  NPRODS
};

enum {
  TOK_SPACE,
  TOK_NAME,
  TOK_ADD,
  TOK_SUB,
  TOK_MUL,
  TOK_DIV,
  TOK_POW,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_COMMA,
  TOK_CHECK,
  TOK_CHOOSE,
  TOK_GT,
  TOK_LT,
  TOK_GEQ,
  TOK_LEQ,
  TOK_EQ,
  TOK_AND,
  TOK_OR,
  TOK_CONST,
  TOK_SEMICOLON,
  TOK_ASSIGN
};

enum { NTOKS = TOK_ASSIGN + 1 };

language build_language();

language_ptr ask_language();

parser_tables_ptr ask_parser_tables();

std::set<std::string> get_variables_used(std::string const& expr);
std::set<std::string> get_symbols_used(std::string const& expr);

}  // end namespace math_lang

}  // end namespace parsegen

#endif
