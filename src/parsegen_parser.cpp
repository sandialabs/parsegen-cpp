#include "parsegen_parser.hpp"

#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm>

#include "parsegen_string.hpp"
#include "parsegen_error.hpp"

namespace parsegen {

void get_line_column(
    std::istream& stream,
    stream_position position,
    int& line,
    int& column)
{
  line = 1;
  column = 1;
  stream.clear(std::ios_base::badbit);
  stream.clear(std::ios_base::failbit);
  stream.clear(std::ios_base::eofbit);
  stream.seekg(0);
  if (stream.tellg() == position) return;
  char c;
  while (stream.get(c)) {
    if (c == '\n') {
      ++line;
      column = 1;
    } else {
      ++column;
    }
    if (stream.tellg() == position) return;
  }
}

void get_underlined_portion(
    std::istream& stream,
    stream_position first,
    stream_position last,
    std::ostream& output)
{
  stream.clear(std::ios_base::badbit);
  stream.clear(std::ios_base::failbit);
  stream.clear(std::ios_base::eofbit);
  stream_position output_first = first;
  stream.seekg(output_first);
  while (true) {
    if (output_first == 0) {
      break;
    }
    stream.seekg(-1, std::ios_base::cur);
    char c;
    if (stream.get(c)) {
      if (c == '\n') {
        output_first = stream.tellg();
        break;
      }
      stream.seekg(-1, std::ios_base::cur);
      output_first = stream.tellg();
    } else {
      throw std::logic_error("stream.get() failed in get_underlined_portion");
    }
  }
  stream_position line_start = output_first;
  stream_position position;
  char c;
  bool last_was_newline;
  while (stream.get(c)) {
    last_was_newline = false;
    output.put(c);
    position = stream.tellg();
    if (c == '\n') {
      last_was_newline = true;
      auto distance = position - line_start;
      for (decltype(distance) i = 0; i < distance; ++i) {
        auto const underline_position = line_start + i;
        if (first <= underline_position && underline_position < last) {
          output.put('~');
        } else {
          output.put(' ');
        }
      }
      output.put('\n');
      line_start = position;
    }
    if (position >= last && c == '\n') {
      break;
    }
  }
  if (!last_was_newline) {
    output.put('\n');
    auto distance = position - line_start;
    for (decltype(distance) i = 0; i < distance; ++i) {
      auto const underline_position = line_start + i;
      if (first <= underline_position && underline_position < last) {
        output.put('~');
      } else {
        output.put(' ');
      }
    }
    output.put('\n');
  }
}

void parser::handle_unacceptable_token(std::istream& stream)
{
  std::stringstream ss;
  int line, column;
  get_line_column(stream, stream_ends_stack.back(), line, column);
  ss << "Could not parse the text\n";
  ss << "at line " << line << " of " << stream_name << ":\n";
  get_underlined_portion(stream, stream_ends_stack.back(), last_lexer_accept_position, ss);
  throw unacceptable_token(ss.str());
}

void parser::handle_reduce_exception(
    std::istream& stream,
    error& e,
    grammar::production const& prod)
{
  std::stringstream ss;
  int line, column;
  get_line_column(stream, stream_ends_stack.back(), line, column);
  ss << "\nat line " << line << " of " << stream_name << ":\n";
  auto const first_stack_index = isize(symbol_stack) - isize(prod.rhs);
  auto const last_stack_index = isize(symbol_stack);
  auto const first_stream_pos = at(stream_ends_stack, first_stack_index);
  auto const last_stream_pos = at(stream_ends_stack, last_stack_index);
  get_underlined_portion(stream, first_stream_pos, last_stream_pos, ss);
  e.set_parser_message(ss.str());
  throw;
}

void parser::handle_shift_exception(std::istream& stream, error& e)
{
  std::stringstream ss;
  int line, column;
  get_line_column(stream, stream_ends_stack.back(), line, column);
  ss << "\nat line " << line << " of " << stream_name << ":\n";
  get_underlined_portion(stream, stream_ends_stack.back(), last_lexer_accept_position, ss);
  e.set_parser_message(ss.str());
  throw;
}

void parser::handle_bad_character(std::istream& stream, char c)
{
  std::stringstream ss;
  int line, column;
  get_line_column(stream, position, line, column);
  ss << "Encountered a non-ASCII character ";
  ss << "at line " << line << ", column " << column << " of " << stream_name << ".\n";
  ss << "This parser can only handle ASCII characters.\n";
  ss << "Usually, non-ASCII characters are caused by trying to ";
  ss << "use foreign-language accents or by copying text from a web page.\n";
  throw bad_character(ss.str());
}

void parser::at_token(std::istream& stream) {
  bool done = false;
  /* this can loop arbitrarily as reductions are made,
     because they don't consume the token */
  while (!done) {
    auto parser_action = get_action(syntax_tables, parser_state, lexer_token);
    if (parser_action.kind == action::kind::none) {
      handle_unacceptable_token(stream);
    } else if (parser_action.kind == action::kind::shift) {
      std::any shift_result;
      try {
        shift_result = this->shift(lexer_token, lexer_text);
      } catch (error& e) {
        handle_shift_exception(stream, e);
      }
      value_stack.emplace_back(std::move(shift_result));
      stream_ends_stack.push_back(last_lexer_accept_position);
      symbol_stack.push_back(lexer_token);
      done = true;
    } else if (parser_action.kind == action::kind::reduce) {
      if (parser_action.production == get_accept_production(*grammar)) {
        did_accept = true;
        return;
      }
      auto& prod = at(grammar->productions, parser_action.production);
      reduction_rhs.clear();
      for (int i = 0; i < isize(prod.rhs); ++i) {
        reduction_rhs.emplace_back(
            std::move(at(value_stack, isize(value_stack) - isize(prod.rhs) + i)));
      }
      std::any reduce_result;
      try {
        reduce_result =
            this->reduce(parser_action.production, reduction_rhs);
      } catch (error& e) {
        handle_reduce_exception(stream, e, prod);
      }
      resize(value_stack, isize(value_stack) - isize(prod.rhs));
      value_stack.emplace_back(std::move(reduce_result));
      auto const old_end = stream_ends_stack.back();
      resize(stream_ends_stack, isize(stream_ends_stack) - isize(prod.rhs));
      stream_ends_stack.push_back(old_end);
      resize(symbol_stack, isize(symbol_stack) - isize(prod.rhs));
      symbol_stack.push_back(prod.lhs);
    } else {
      throw std::logic_error(
          "serious bug in parsegen::parser: action::kind enum value out of range\n");
    }
    parser_state = execute_action(syntax_tables, parser_stack, parser_action);
  }
}

void parser::handle_indent_mismatch(std::istream& stream) {
  std::stringstream ss;
  int line, column;
  get_line_column(stream, last_lexer_accept_position, line, column);
  ss << "The indentation characters beginning line " << line << " of "
     << stream_name << " do not match earlier indentation.\n";
  throw error("", ss.str());
}

void parser::at_token_indent(std::istream& stream) {
  if (!sensing_indent || lexer_token != tables->indent_info.newline_token) {
    at_token(stream);
    return;
  }
  auto last_newline_pos = lexer_text.find_last_of("\n");
  if (last_newline_pos == std::string::npos) {
    throw error("", "INDENT token did not contain a newline");
  }
  auto lexer_indent =
      lexer_text.substr(last_newline_pos + 1, std::string::npos);
  // the at_token call is allowed to do anything to lexer_text
  at_token(stream);
  lexer_text.clear();
  std::size_t minlen = std::min(lexer_indent.length(), indent_text.length());
  if (lexer_indent.length() > indent_text.length()) {
    if (0 != lexer_indent.compare(0, indent_text.length(), indent_text)) {
      handle_indent_mismatch(stream);
    }
    indent_stack.push_back({indent_text.length(), lexer_indent.length()});
    indent_text = lexer_indent;
    lexer_token = tables->indent_info.indent_token;
    at_token(stream);
  } else if (lexer_indent.length() < indent_text.length()) {
    if (0 != indent_text.compare(0, lexer_indent.length(), lexer_indent)) {
      handle_indent_mismatch(stream);
    }
    while (!indent_stack.empty()) {
      auto top = indent_stack.back();
      if (top.end_length <= minlen) break;
      indent_stack.pop_back();
      lexer_token = tables->indent_info.dedent_token;
      at_token(stream);
    }
    indent_text = lexer_indent;
  } else {
    if (0 != lexer_indent.compare(indent_text)) {
      handle_indent_mismatch(stream);
    }
  }
}

void parser::backtrack_to_last_accept(std::istream& stream) {
  /* all the last_accept and backtracking is driven by
    the "accept the longest match" rule */
  lexer_text.resize(last_lexer_accept);
  stream.seekg(last_lexer_accept_position);
}

void parser::reset_lexer_state() {
  lexer_state = 0;
  lexer_text.clear();
  lexer_token = -1;
}

void parser::print_parser_stack(std::istream& stream, std::ostream& output)
{
  output << "The parser stack contains:\n";
  for (int i = 0; i < isize(symbol_stack); ++i) {
    output << at(grammar->symbol_names, at(symbol_stack, i)) << ":\n";
    if (i + 1 >= isize(stream_ends_stack)) {
      throw std::logic_error("i + 1 >= isize(stream_ends_stack)!");
    }
    auto const first = at(stream_ends_stack, i);
    auto const last = at(stream_ends_stack, i + 1);
    get_underlined_portion(stream, first, last, output);
    output << '\n';
  }
}

void parser::handle_tokenization_failure(std::istream& stream)
{
  std::stringstream ss;
  int line, column;
  get_line_column(stream, last_lexer_accept_position, line, column);
  ss << "Could not parse the text\n";
  ss << "at line " << line << " of " << stream_name << ":\n";
  get_underlined_portion(stream, last_lexer_accept_position, position, ss);
  throw tokenization_failure(ss.str());
}

void parser::at_lexer_end(std::istream& stream) {
  if (lexer_token == -1) {
    handle_tokenization_failure(stream);
  }
  backtrack_to_last_accept(stream);
  at_token_indent(stream);
  reset_lexer_state();
}

parser::parser(parser_tables_ptr tables_in)
    : tables(tables_in),
      syntax_tables(tables->syntax_tables),
      lexical_tables(tables->lexical_tables),
      grammar(get_grammar(syntax_tables))
{
  if (!get_determinism(lexical_tables)) {
    throw std::logic_error("parsegen::parser: the lexer in the given tables is not a deterministic finite automaton");
  }
}

std::any parser::parse_stream(
    std::istream& stream, std::string const& stream_name_in) {
  lexer_state = 0;
  lexer_text.clear();
  lexer_token = -1;
  parser_state = 0;
  parser_stack.clear();
  parser_stack.push_back(parser_state);
  value_stack.clear();
  stream_ends_stack.clear();
  stream_ends_stack.push_back(stream.tellg());
  symbol_stack.clear();
  did_accept = false;
  stream_name = stream_name_in;
  if (tables->indent_info.is_sensitive) {
    sensing_indent = true;
    indent_text.clear();
    indent_stack.clear();
  } else {
    sensing_indent = false;
  }
  char c;
  while (stream.get(c)) {
    if (!is_symbol(c)) {
      handle_bad_character(stream, c);
    }
    position = stream.tellg();
    lexer_text.push_back(c);
    auto lexer_symbol = get_symbol(c);
    lexer_state = step(lexical_tables, lexer_state, lexer_symbol);
    if (lexer_state == -1) {
      at_lexer_end(stream);
    } else {
      auto token = accepts(lexical_tables, lexer_state);
      if (token != -1) {
        lexer_token = token;
        last_lexer_accept = lexer_text.size();
        last_lexer_accept_position = stream.tellg();
      }
    }
  }
  if (last_lexer_accept < lexer_text.size()) {
    handle_tokenization_failure(stream);
  }
  at_lexer_end(stream);
  lexer_token = get_end_terminal(*grammar);
  at_token(stream);
  if (!did_accept) {
    throw std::logic_error(
        "The EOF terminal was accepted but the root nonterminal was not "
        "reduced\n"
        "This indicates a bug in parsegen::parser\n");
  }
  if (value_stack.size() != 1) {
    throw std::logic_error(
        "parsegen::parser::parse_stream finished but value_stack has size "
        + std::to_string(value_stack.size())
        + "\nThis indicates a bug in parsegen::parser\n");
  }
  return std::move(value_stack.back());
}

std::any parser::parse_string(
    std::string const& string, std::string const& string_name) {
  std::istringstream stream(string);
  return parse_stream(stream, string_name);
}

std::any parser::parse_file(std::filesystem::path const& file_path) {
  std::ifstream stream(file_path);
  if (!stream.is_open()) {
    throw error("Could not open file " + file_path.string());
  }
  return parse_stream(stream, file_path.string());
}

std::any parser::shift(int, std::string&) { return std::any(); }

std::any parser::reduce(int, std::vector<std::any>&) { return std::any(); }

debug_parser::debug_parser(parser_tables_ptr tables_in, std::ostream& os_in)
    : parser(tables_in), os(os_in) {}

std::any debug_parser::shift(int token, std::string& text) {
  auto escaped_text = escape_for_c_string(text);
  os << "SHIFT (" << at(grammar->symbol_names, token) << ")["
    << escaped_text << "]\n";
  return escaped_text;
}

std::any debug_parser::reduce(int prod_i, std::vector<std::any>& rhs) {
  os << "REDUCE";
  std::string lhs_text;
  auto& prod = at(grammar->productions, prod_i);
  for (int i = 0; i < isize(prod.rhs); ++i) {
    auto& rhs_name = at(grammar->symbol_names, at(prod.rhs, i));
    auto rhs_text = std::any_cast<std::string&&>(std::move(at(rhs, i)));
    os << " (" << rhs_name << ")[" << rhs_text << "]";
    lhs_text.append(rhs_text);
  }
  auto& lhs_name = at(grammar->symbol_names, prod.lhs);
  os << " -> (" << lhs_name << ")[" << lhs_text << "]\n";
  return std::any(std::move(lhs_text));
}

}  // end namespace parsegen
