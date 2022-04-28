#ifndef PARSEGEN_PARSER_HPP
#define PARSEGEN_PARSER_HPP

#include <filesystem>
#include <functional>
#include <iosfwd>
#include <any>

#include "parsegen_parser_tables.hpp"
#include "parsegen_std_vector.hpp"

namespace parsegen {

using stream_position = std::istream::pos_type;

class parser {
 public:
  parser() = delete;
  parser(parser const&) = default;
  virtual ~parser() = default;
  parser(parser_tables_ptr tables_in);
  std::any parse_stream(
      std::istream& stream,
      std::string const& stream_name_in = "");
  std::any parse_string(
      std::string const& string,
      std::string const& string_name = "");
  std::any parse_file(
      std::filesystem::path const& file_path);

 protected:
  virtual std::any shift(int token, std::string& text);
  virtual std::any reduce(int production, std::vector<std::any>& rhs);

 protected:
  parser_tables_ptr tables;
  shift_reduce_tables const& syntax_tables;
  finite_automaton const& lexical_tables;
  grammar_ptr grammar;
  stream_position position;
  int lexer_state;
  std::string lexer_text;
  int lexer_token;
  std::size_t last_lexer_accept;
  stream_position last_lexer_accept_position;
  int parser_state;
  std::vector<int> parser_stack;
  std::vector<std::any> value_stack;
  std::vector<std::any> reduction_rhs;
  std::vector<stream_position> stream_ends_stack;
  std::vector<int> symbol_stack;
  std::string stream_name;
  bool did_accept;

 protected:  // variables for indentation-sensitive language parsing
  bool sensing_indent;
  std::string indent_text;
  struct indent_stack_entry {
    std::size_t start_length;
    std::size_t end_length;
  };
  // this is the stack that shows, for the current leading indentation
  // characters, which subset of them came from each nested increase
  // in indentation
  std::vector<indent_stack_entry> indent_stack;

 private:  // helper methods
  void at_token(std::istream& stream);
  void at_token_indent(std::istream& stream);
  void at_lexer_end(std::istream& stream);
  void backtrack_to_last_accept(std::istream& stream);
  void reset_lexer_state();
  void print_parser_stack(std::istream& stream, std::ostream& output);
  [[noreturn]] void handle_tokenization_failure(std::istream& stream);
  [[noreturn]] void handle_unacceptable_token(std::istream& stream);
  [[noreturn]] void handle_reduce_exception(std::istream& stream, std::exception const& e, int production);
  [[noreturn]] void handle_shift_exception(std::istream& stream, std::exception const& e);
  [[noreturn]] void handle_bad_character(std::istream& stream, char c);
  [[noreturn]] void handle_indent_mismatch(std::istream& stream);
};

class debug_parser : public parser {
 public:
  debug_parser(parser_tables_ptr tables_in, std::ostream& os_in);
  debug_parser(debug_parser const& other) = default;
  virtual ~debug_parser() override = default;

 protected:
  virtual std::any shift(int token, std::string& text) override;
  virtual std::any reduce(int production, std::vector<std::any>& rhs) override;

 private:
  std::ostream& os;
};

}  // namespace parsegen

#endif
