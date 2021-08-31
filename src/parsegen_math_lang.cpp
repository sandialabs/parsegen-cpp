#include "parsegen_math_lang.hpp"
#include "parsegen_parser.hpp"

namespace parsegen {

namespace math_lang {

language build_language() {
  language out;
  auto& prods = out.productions;
  prods.resize(NPRODS);
  prods[PROD_PROGRAM] = {"program", {"statements", "expr?"}};
  prods[PROD_NO_STATEMENTS] = {"statements", {}};
  prods[PROD_NEXT_STATEMENT] = {
      "statements", {"statements", "statement", ";", "S?"}};
  prods[PROD_ASSIGN] = {"statement", {"name", "S?", "=", "S?", "expr"}};
  prods[PROD_NO_EXPR] = {"expr?", {}};
  prods[PROD_YES_EXPR] = {"expr?", {"expr"}};
  prods[PROD_EXPR] = {"expr", {"ternary"}};
  prods[PROD_TERNARY_DECAY] = {"ternary", {"add_sub"}};
  prods[PROD_OR_DECAY] = {"or", {"and"}};
  prods[PROD_AND_DECAY] = {"and", {"comp"}};
  prods[PROD_ADD_SUB_DECAY] = {"add_sub", {"mul_div"}};
  prods[PROD_MUL_DIV_DECAY] = {"mul_div", {"neg"}};
  prods[PROD_NEG_DECAY] = {"neg", {"pow"}};
  prods[PROD_POW_DECAY] = {"pow", {"scalar"}};
  prods[PROD_TERNARY] = {
      "ternary", {"or", "?", "S?", "add_sub", ":", "S?", "add_sub"}};
  prods[PROD_OR] = {"or", {"or", "||", "S?", "and"}};
  prods[PROD_AND] = {"and", {"and", "&&", "S?", "comp"}};
  prods[PROD_GT] = {"comp", {"add_sub", ">", "S?", "add_sub"}};
  prods[PROD_LT] = {"comp", {"add_sub", "<", "S?", "add_sub"}};
  prods[PROD_GEQ] = {"comp", {"add_sub", ">=", "S?", "add_sub"}};
  prods[PROD_LEQ] = {"comp", {"add_sub", "<=", "S?", "add_sub"}};
  prods[PROD_EQ] = {"comp", {"add_sub", "==", "S?", "add_sub"}};
  prods[PROD_BOOL_PARENS] = {"comp", {"(", "S?", "or", ")", "S?"}};
  prods[PROD_ADD] = {"add_sub", {"add_sub", "+", "S?", "mul_div"}};
  prods[PROD_SUB] = {"add_sub", {"add_sub", "-", "S?", "mul_div"}};
  prods[PROD_MUL] = {"mul_div", {"mul_div", "*", "S?", "pow"}};
  prods[PROD_DIV] = {"mul_div", {"mul_div", "/", "S?", "pow"}};
  prods[PROD_POW] = {"pow", {"scalar", "^", "S?", "pow"}};
  prods[PROD_CALL] = {"scalar", {"name", "S?", "(", "S?", "args?", ")", "S?"}};
  prods[PROD_NO_ARGS] = {"args?", {}};
  prods[PROD_SOME_ARGS] = {"args?", {"args"}};
  prods[PROD_FIRST_ARG] = {"args", {"ternary"}};
  prods[PROD_NEXT_ARG] = {"args", {"args", ",", "S?", "ternary"}};
  prods[PROD_NEG] = {"neg", {"-", "S?", "neg"}};
  prods[PROD_VAL_PARENS] = {"scalar", {"(", "S?", "ternary", ")", "S?"}};
  prods[PROD_CONST] = {"scalar", {"constant", "S?"}};
  prods[PROD_VAR] = {"scalar", {"name", "S?"}};
  prods[PROD_NO_SPACES] = {"S?", {}};
  prods[PROD_SPACES] = {"S?", {"spaces"}};
  out.tokens.resize(NTOKS);
  out.tokens[TOK_SPACE] = {"spaces", "[ \t\n\r]+"};
  out.tokens[TOK_NAME] = {"name", "[_a-zA-Z][_a-zA-Z0-9]*"};
  out.tokens[TOK_ADD] = {"+", "\\+"};
  out.tokens[TOK_SUB] = {"-", "\\-"};
  out.tokens[TOK_MUL] = {"*", "\\*"};
  out.tokens[TOK_DIV] = {"/", "\\/"};
  out.tokens[TOK_POW] = {"^", "\\^"};
  out.tokens[TOK_LPAREN] = {"(", "\\("};
  out.tokens[TOK_RPAREN] = {")", "\\)"};
  out.tokens[TOK_COMMA] = {",", ","};
  out.tokens[TOK_CHECK] = {"?", "\\?"};
  out.tokens[TOK_CHOOSE] = {":", ":"};
  out.tokens[TOK_GT] = {">", ">"};
  out.tokens[TOK_LT] = {"<", "<"};
  out.tokens[TOK_GEQ] = {">=", ">="};
  out.tokens[TOK_LEQ] = {"<=", "<="};
  out.tokens[TOK_EQ] = {"==", "=="};
  out.tokens[TOK_AND] = {"&&", "&&"};
  out.tokens[TOK_OR] = {"||", "\\|\\|"};
  out.tokens[TOK_CONST] = {
      "constant", "(0|([1-9][0-9]*))(\\.[0-9]*)?([eE][\\-\\+]?[0-9]+)?"};
  out.tokens[TOK_SEMICOLON] = {";", ";"};
  out.tokens[TOK_ASSIGN] = {"=", "="};
  return out;
}

language_ptr ask_language() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
  static language_ptr ptr;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  if (ptr.use_count() == 0) {
    ptr.reset(new language(build_language()));
  }
  return ptr;
}

parser_tables_ptr ask_parser_tables() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
  static parser_tables_ptr ptr;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  if (ptr.use_count() == 0) {
    language_ptr lang = ask_language();
    ptr = build_parser_tables(*lang);
  }
  return ptr;
}

class symbols_parser : public parser {
 public:
  symbols_parser();
  ~symbols_parser() override = default;

 public:
  std::set<std::string> variable_names;
  std::set<std::string> function_names;

 private:
  std::any shift(int token, std::string& text) override;
  std::any reduce(int prod, std::vector<std::any>& rhs) override;
};

symbols_parser::symbols_parser() : parser(ask_parser_tables()) {}

std::any symbols_parser::shift(int token, std::string& text) {
  if (token == TOK_NAME) return text;
  return std::any();
}

std::any symbols_parser::reduce(int prod, std::vector<std::any>& rhs) {
  if (prod == PROD_VAR) {
    auto& name = std::any_cast<std::string&>(rhs.at(0));
    variable_names.insert(name);
  } else if (prod == PROD_CALL) {
    auto& name = std::any_cast<std::string&>(rhs.at(0));
    function_names.insert(name);
  }
  return std::any();
}

std::set<std::string> get_variables_used(std::string const& expr) {
  symbols_parser parser;
  parser.read_string(expr, "get_variables_used");
  return parser.variable_names;
}

std::set<std::string> get_symbols_used(std::string const& expr) {
  symbols_parser parser;
  parser.read_string(expr, "get_symbols_used");
  auto set = std::move(parser.variable_names);
  set.insert(parser.function_names.begin(), parser.function_names.end());
  return set;
}

}  // end namespace math_lang

}  // namespace parsegen
