#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "parsegen.hpp"

namespace {

class calculator : public parsegen::parser {
 public:
  calculator() : parsegen::parser(parsegen::math_lang::ask_parser_tables()) {
    unary_function_map["sqrt"] = &std::sqrt;
    unary_function_map["sin"] = &std::sin;
    unary_function_map["cos"] = &std::cos;
    unary_function_map["tan"] = &std::tan;
    unary_function_map["asin"] = &std::asin;
    unary_function_map["acos"] = &std::acos;
    unary_function_map["atan"] = &std::atan;
    unary_function_map["exp"] = &std::exp;
    unary_function_map["log"] = &std::log;
    unary_function_map["log10"] = &std::log10;
    binary_function_map["atan2"] = &std::atan2;
  }
  virtual ~calculator() = default;

 protected:
  struct arguments {
    double a0;
    double a1;
    int n;
  };
  virtual std::any shift(int token, std::string& text) override {
    switch (token) {
      case parsegen::math_lang::TOK_NAME: {
        return text;
      }
      case parsegen::math_lang::TOK_CONST: {
        return std::atof(text.c_str());
      }
    }
    return std::any();
  }
  virtual std::any reduce(int prod, std::vector<std::any>& rhs) override {
    using std::any_cast;
    switch (prod) {
      case parsegen::math_lang::PROD_PROGRAM: {
        if (!rhs.at(1).has_value()) {
          throw parsegen::error(
              "Calculator needs an expression to evaluate!");
        }
        return std::move(rhs.at(1));
      }
      case parsegen::math_lang::PROD_NO_STATEMENTS:
      case parsegen::math_lang::PROD_NO_EXPR:
      case parsegen::math_lang::PROD_NEXT_STATEMENT: {
        return std::any();
      }
      case parsegen::math_lang::PROD_ASSIGN: {
        auto& name = any_cast<std::string&>(rhs.at(0));
        double value = any_cast<double>(rhs.at(2));
        variable_map[name] = value;
        return value;
      }
      case parsegen::math_lang::PROD_YES_EXPR:
      case parsegen::math_lang::PROD_EXPR:
      case parsegen::math_lang::PROD_TERNARY_DECAY:
      case parsegen::math_lang::PROD_OR_DECAY:
      case parsegen::math_lang::PROD_AND_DECAY:
      case parsegen::math_lang::PROD_ADD_SUB_DECAY:
      case parsegen::math_lang::PROD_MUL_DIV_DECAY:
      case parsegen::math_lang::PROD_POW_DECAY:
      case parsegen::math_lang::PROD_NEG_DECAY:
      case parsegen::math_lang::PROD_SOME_ARGS:
        return std::move(rhs.at(0));
      case parsegen::math_lang::PROD_TERNARY:
        return any_cast<bool>(rhs.at(0)) ? any_cast<double>(rhs.at(2))
                                         : any_cast<double>(rhs.at(4));
      case parsegen::math_lang::PROD_OR:
        return any_cast<bool>(rhs.at(0)) || any_cast<bool>(rhs.at(2));
      case parsegen::math_lang::PROD_AND:
        return any_cast<bool>(rhs.at(0)) && any_cast<bool>(rhs.at(2));
      case parsegen::math_lang::PROD_GT:
        return any_cast<double>(rhs.at(0)) > any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_LT:
        return any_cast<double>(rhs.at(0)) < any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_GEQ:
        return any_cast<double>(rhs.at(0)) >= any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_LEQ:
        return any_cast<double>(rhs.at(0)) <= any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_EQ:
        return any_cast<double>(rhs.at(0)) == any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_BOOL_PARENS:
        return any_cast<bool>(rhs.at(1));
      case parsegen::math_lang::PROD_ADD:
        return any_cast<double>(rhs.at(0)) + any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_SUB:
        return any_cast<double>(rhs.at(0)) - any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_MUL:
        return any_cast<double>(rhs.at(0)) * any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_DIV:
        return any_cast<double>(rhs.at(0)) / any_cast<double>(rhs.at(2));
      case parsegen::math_lang::PROD_POW:
        return std::pow(
            any_cast<double>(rhs.at(0)), any_cast<double>(rhs.at(2)));
      case parsegen::math_lang::PROD_CALL: {
        auto& name = any_cast<std::string&>(rhs.at(0));
        auto& args = any_cast<arguments&>(rhs.at(2));
        if (args.n < 1 || args.n > 2) {
          throw parsegen::error(
              "Only unary and binary functions supported!\n");
        }
        if (args.n == 1) {
          if (!unary_function_map.count(name)) {
            std::stringstream ss;
            ss << "Unknown unary function name \"" << name << "\"\n";
            throw parsegen::error(ss.str());
          }
          Unary fptr = unary_function_map[name];
          return (*fptr)(args.a0);
        } else {
          if (!binary_function_map.count(name)) {
            std::stringstream ss;
            ss << "Unknown binary function name \"" << name << "\"\n";
            throw parsegen::error(ss.str());
          }
          Binary fptr = binary_function_map[name];
          return (*fptr)(args.a0, args.a1);
        }
      }
      case parsegen::math_lang::PROD_NO_ARGS: {
        arguments args;
        args.n = 0;
        return args;
      }
      case parsegen::math_lang::PROD_FIRST_ARG: {
        arguments args;
        args.a0 = any_cast<double>(rhs.at(0));
        args.n = 1;
        return args;
      }
      case parsegen::math_lang::PROD_NEXT_ARG: {
        auto& args = any_cast<arguments&>(rhs.at(0));
        args.a1 = any_cast<double>(rhs.at(2));
        args.n += 1;
        return args;
      }
      case parsegen::math_lang::PROD_NEG:
        return -any_cast<double>(rhs.at(1));
      case parsegen::math_lang::PROD_VAL_PARENS:
        return any_cast<double>(rhs.at(1));
      case parsegen::math_lang::PROD_CONST:
        return any_cast<double>(rhs.at(0));
      case parsegen::math_lang::PROD_VAR:
        auto& name = any_cast<std::string&>(rhs.at(0));
        auto it = variable_map.find(name);
        if (it == variable_map.end()) {
          std::stringstream ss;
          ss << "variable " << name << " not defined!";
          throw parsegen::error(ss.str());
        }
        return it->second;
    }
    return std::any();
  }

 private:
  typedef double (*Unary)(double);
  typedef double (*Binary)(double, double);
  std::map<std::string, Unary> unary_function_map;
  std::map<std::string, Binary> binary_function_map;
  std::map<std::string, double> variable_map;
};

}  // end anonymous namespace

int main() {
  calculator parser;
  for (std::string line; std::getline(std::cin, line);) {
    std::cout << std::any_cast<double>(parser.parse_string(line, "input"))
              << '\n';
  }
}
