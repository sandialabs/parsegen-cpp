#ifndef PARSEGEN_READER_HPP
#define PARSEGEN_READER_HPP

#include <functional>
#include <iosfwd>
#include <any>

#include "parsegen_reader_tables.hpp"
#include "parsegen_std_vector.hpp"

namespace parsegen {

class reader {
 public:
  reader() = delete;
  reader(reader const&) = default;
  virtual ~reader() = default;
  reader(reader_tables_ptr tables_in);
  std::any read_stream(std::istream& stream, std::string const& stream_name_in = "");
  std::any read_string(
      std::string const& string, std::string const& string_name = "");
  std::any read_file(std::string const& file_name);

 protected:
  virtual std::any at_shift(int token, std::string& text);
  virtual std::any at_reduce(int token, std::vector<std::any>& rhs);

 protected:
  reader_tables_ptr tables;
  parser const& parser;
  finite_automaton const& lexer;
  grammar_ptr grammar;
  std::size_t line;
  std::size_t column;
  int lexer_state;
  std::string lexer_text;
  std::string line_text;
  int lexer_token;
  std::size_t last_lexer_accept;
  std::size_t last_lexer_accept_line;
  std::size_t last_lexer_accept_column;
  std::string last_lexer_accept_line_text;
  int parser_state;
  std::vector<int> parser_stack;
  std::vector<std::any> value_stack;
  std::vector<std::any> reduction_rhs;
  std::string stream_name;
  bool did_accept;

 protected:  // variables for indentation-sensitive language parsing
  bool sensing_indent;
  std::string indent_text;
  struct indent_stack_entry {
    std::size_t line;
    std::size_t start_length;
    std::size_t end_length;
  };
  // this is the stack that shows, for the current leading indentation
  // characters, which subset of them came from each nested increase
  // in indentation
  std::vector<indent_stack_entry> indent_stack;
  // this stack notes, for each symbol in the pushdown automaton
  // stack, how many characters indent the line that that symbol
  // starts on
  std::vector<std::size_t> symbol_indentation_stack;

 private:  // helper methods
  void at_token(std::istream& stream);
  [[noreturn]] void indent_mismatch();
  void at_token_indent(std::istream& stream);
  void at_lexer_end(std::istream& stream);
  void backtrack_to_last_accept(std::istream& stream);
  void reset_lexer_state();
  void update_position(char c);
  void error_print_line(std::istream& is, std::ostream& os);
};

class debug_reader : public reader {
 public:
  debug_reader(reader_tables_ptr tables_in, std::ostream& os_in);
  debug_reader(debug_reader const& other) = default;
  virtual ~debug_reader() override = default;

 protected:
  virtual std::any at_shift(int token, std::string& text) override;
  virtual std::any at_reduce(int token, std::vector<std::any>& rhs) override;

 private:
  std::ostream& os;
};

}  // namespace parsegen

#endif
