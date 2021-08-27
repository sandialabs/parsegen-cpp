#include "parsegen_reader.hpp"

#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm>

#include "parsegen_string.hpp"

namespace parsegen {

namespace {

void print_indicator(
    std::ostream& os, std::string const& above, std::size_t pos) {
  for (std::size_t i = 0; i < pos; ++i) {
    if (above.at(i) == '\t')
      os << '\t';
    else
      os << ' ';
  }
  os << "^\n";
}

void print_underline(std::ostream& os, std::string const& above,
    std::size_t start, std::size_t end) {
  for (std::size_t i = 0; i < start; ++i) {
    if (above.at(i) == '\t')
      os << '\t';
    else
      os << ' ';
  }
  for (auto i = start; i < end; ++i) os << '~';
  os << '\n';
}

}  // end anonymous namespace

void reader::at_token(std::istream& stream) {
  bool done = false;
  /* this can loop arbitrarily as reductions are made,
     because they don't consume the token */
  while (!done) {
    auto parser_action = get_action(parser, parser_state, lexer_token);
    if (parser_action.kind == ACTION_NONE) {
      std::stringstream ss;
      ss << "error: parser failure at line " << line;
      ss << " column " << column << " of " << stream_name << '\n';
      error_print_line(stream, ss);
      std::set<std::string> expect_names;
      for (int expect_token = 0; expect_token < grammar->nterminals;
           ++expect_token) {
        auto expect_action = get_action(parser, parser_state, expect_token);
        if (expect_action.kind != ACTION_NONE) {
          expect_names.insert(at(grammar->symbol_names, expect_token));
        }
      }
      ss << "Expected one of {";
      for (std::set<std::string>::iterator it = expect_names.begin();
           it != expect_names.end(); ++it) {
        if (it != expect_names.begin()) ss << ", ";
        if (*it == ",")
          ss << "','";
        else
          ss << *it;
      }
      ss << "}\n";
      ss << "Got: " << at(grammar->symbol_names, lexer_token) << '\n';
      ss << "Lexer text: \"" << lexer_text << "\"\n";
      ss << "parser was in state " << parser_state << '\n';
      throw parse_error(ss.str());
    } else if (parser_action.kind == ACTION_SHIFT) {
      if (sensing_indent) {
        symbol_indentation_stack.push_back(indent_text.size());
      }
      std::any shift_result;
      shift_result = this->at_shift(lexer_token, lexer_text);
      value_stack.emplace_back(std::move(shift_result));
      stream_ends_stack.push_back(last_lexer_accept_position);
      symbol_stack.push_back(lexer_token);
      done = true;
    } else if (parser_action.kind == ACTION_REDUCE) {
      if (parser_action.production == get_accept_production(*grammar)) {
        did_accept = true;
        return;
      }
      auto& prod = at(grammar->productions, parser_action.production);
      reduction_rhs.clear();
      for (int i = 0; i < size(prod.rhs); ++i) {
        reduction_rhs.emplace_back(
            std::move(at(value_stack, size(value_stack) - size(prod.rhs) + i)));
      }
      resize(value_stack, size(value_stack) - size(prod.rhs));
      std::any reduce_result;
      try {
        reduce_result =
            this->at_reduce(parser_action.production, reduction_rhs);
      } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "error: parser failure at line " << line;
        ss << " column " << column << " of " << stream_name << '\n';
        error_print_line(stream, ss);
        ss << '\n' << e.what();
        throw parse_error(ss.str());
      }
      value_stack.emplace_back(std::move(reduce_result));
      auto const old_end = stream_ends_stack.back();
      resize(stream_ends_stack, size(stream_ends_stack) - size(prod.rhs));
      stream_ends_stack.push_back(old_end);
      resize(symbol_stack, size(symbol_stack) - size(prod.rhs));
      symbol_stack.push_back(prod.lhs);
      if (sensing_indent) {
        if (size(prod.rhs)) {
          resize(symbol_indentation_stack,
              (size(symbol_indentation_stack) + 1) - size(prod.rhs));
        } else {
          symbol_indentation_stack.push_back(symbol_indentation_stack.back());
        }
      }
    } else {
      throw std::logic_error(
          "serious bug in parsegen::reader: action::kind enum value out of range\n");
    }
    parser_state = execute_action(parser, parser_stack, parser_action);
  }
}

void reader::indent_mismatch() {
  if (indent_stack.empty()) {
    throw std::logic_error(
        "parsegen::reader detected an indentation mismatch but the indent_stack is empty\n"
        "This indicates a bug in parsegen::reader\n");
  }
  auto top = indent_stack.back();
  std::stringstream ss;
  ss << "error: Indentation characters beginning line " << line << " of "
     << stream_name << " don't match those beginning line " << top.line << '\n';
  ss << "It is strongly recommended not to mix tabs and spaces in "
        "indentation-sensitive formats\n";
  throw parse_error(ss.str());
}

void reader::at_token_indent(std::istream& stream) {
  if (!sensing_indent || lexer_token != tables->indent_info.newline_token) {
    at_token(stream);
    return;
  }
  auto last_newline_pos = lexer_text.find_last_of("\n");
  if (last_newline_pos == std::string::npos) {
    throw parse_error("INDENT token did not contain a newline");
  }
  auto lexer_indent =
      lexer_text.substr(last_newline_pos + 1, std::string::npos);
  // the at_token call is allowed to do anything to lexer_text
  at_token(stream);
  lexer_text.clear();
  std::size_t minlen = std::min(lexer_indent.length(), indent_text.length());
  if (lexer_indent.length() > indent_text.length()) {
    if (0 != lexer_indent.compare(0, indent_text.length(), indent_text)) {
      indent_mismatch();
    }
    indent_stack.push_back({line, indent_text.length(), lexer_indent.length()});
    indent_text = lexer_indent;
    lexer_token = tables->indent_info.indent_token;
    at_token(stream);
  } else if (lexer_indent.length() < indent_text.length()) {
    if (0 != indent_text.compare(0, lexer_indent.length(), lexer_indent)) {
      indent_mismatch();
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
      indent_mismatch();
    }
  }
}

void reader::backtrack_to_last_accept(std::istream& stream) {
  /* all the last_accept and backtracking is driven by
    the "accept the longest match" rule */
  line = last_lexer_accept_line;
  column = last_lexer_accept_column;
  line_text = last_lexer_accept_line_text;
  lexer_text.resize(last_lexer_accept);
  stream.seekg(last_lexer_accept_position);
}

void reader::reset_lexer_state() {
  lexer_state = 0;
  lexer_text.clear();
  lexer_token = -1;
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
    stream.unget();
    char c;
    if (stream.get(c)) {
      if (c == '\n') {
        output_first = stream.tellg();
        break;
      }
      stream.unget();
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

void reader::print_parser_stack(std::istream& stream, std::ostream& output)
{
  output << "parser stack is:\n";
  for (int i = 0; i < size(symbol_stack); ++i) {
    output << at(grammar->symbol_names, at(symbol_stack, i)) << ":\n";
    if (i + 1 >= size(stream_ends_stack)) {
      throw std::logic_error("i + 1 >= size(stream_ends_stack)!");
    }
    auto const first = at(stream_ends_stack, i);
    auto const last = at(stream_ends_stack, i + 1);
    get_underlined_portion(stream, first, last, output);
    output << '\n';
  }
}

void reader::handle_tokenization_failure(std::istream& stream)
{
  std::stringstream ss;
  ss << "parsegen::reader found some text that did not match any of the tokens in the language:\n";
  get_underlined_portion(stream, last_lexer_accept_position, position, ss);
  print_parser_stack(stream, ss);
  throw parse_error(ss.str());
}

void reader::at_lexer_end(std::istream& stream) {
  if (lexer_token == -1) {
    handle_tokenization_failure(stream);
  }
  backtrack_to_last_accept(stream);
  at_token_indent(stream);
  reset_lexer_state();
}

reader::reader(reader_tables_ptr tables_in)
    : tables(tables_in),
      parser(tables->parser),
      lexer(tables->lexer),
      grammar(get_grammar(parser))
{
  if (!get_determinism(lexer)) {
    throw std::logic_error("parsegen::reader: the lexer in the given tables is not a deterministic finite automaton");
  }
}

void reader::update_position(char c) {
  if (c == '\n') {
    ++line;
    column = 1;
    line_text.clear();
  } else {
    ++column;
  }
}

void reader::error_print_line(std::istream& is, std::ostream& os) {
  auto oldpos = line_text.size();
  char c;
  while (is.get(c)) {
    if (c == '\n' || c == '\r') break;
    line_text.push_back(c);
  }
  if (line_text.empty()) return;
  os << line_text << '\n';
  if (oldpos > 0) print_indicator(os, line_text, oldpos - 1);
}

std::any reader::read_stream(
    std::istream& stream, std::string const& stream_name_in) {
  line = 1;
  column = 1;
  lexer_state = 0;
  lexer_text.clear();
  line_text.clear();
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
      std::stringstream ss;
      ss << "error: Unexpected ASCII code " << int(c);
      ss << " at line " << line << " column " << column;
      ss << " of " << stream_name << '\n';
      error_print_line(stream, ss);
      throw parse_error(ss.str());
    }
    position = stream.tellg();
    line_text.push_back(c);
    lexer_text.push_back(c);
    auto lexer_symbol = get_symbol(c);
    lexer_state = step(lexer, lexer_state, lexer_symbol);
    if (lexer_state == -1) {
      at_lexer_end(stream);
    } else {
      auto token = accepts(lexer, lexer_state);
      update_position(c);
      if (token != -1) {
        lexer_token = token;
        last_lexer_accept = lexer_text.size();
        last_lexer_accept_line = line;
        last_lexer_accept_column = column;
        last_lexer_accept_line_text = line_text;
        last_lexer_accept_position = stream.tellg();
      }
    }
  }
  if (last_lexer_accept < lexer_text.size()) {
    std::stringstream ss;
    std::string bad_str =
        lexer_text.substr(last_lexer_accept, std::string::npos);
    ss << "error: Could not tokenize \"" << bad_str;
    ss << "\" at end of " << stream_name << '\n';
    throw parse_error(ss.str());
  }
  at_lexer_end(stream);
  lexer_token = get_end_terminal(*grammar);
  at_token(stream);
  if (!did_accept) {
    throw std::logic_error(
        "The EOF terminal was accepted but the root nonterminal was not "
        "reduced\n"
        "This indicates a bug in parsegen::reader\n");
  }
  if (value_stack.size() != 1) {
    throw std::logic_error(
        "parsegen::reader::read_stream finished but value_stack has size "
        + std::to_string(value_stack.size())
        + "\nThis indicates a bug in parsegen::reader\n");
  }
  return std::move(value_stack.back());
}

std::any reader::read_string(
    std::string const& string, std::string const& string_name) {
  std::istringstream stream(string);
  return read_stream(stream, string_name);
}

std::any reader::read_file(std::filesystem::path const& file_path) {
  std::ifstream stream(file_path);
  if (!stream.is_open()) {
    std::stringstream ss;
    ss << "Could not open file " << file_path;
    throw parse_error(ss.str());
  }
  return read_stream(stream, file_path.string());
}

std::any reader::at_shift(int, std::string&) { return std::any(); }

std::any reader::at_reduce(int, std::vector<std::any>&) { return std::any(); }

debug_reader::debug_reader(reader_tables_ptr tables_in, std::ostream& os_in)
    : reader(tables_in), os(os_in) {}

std::any debug_reader::at_shift(int token, std::string& text) {
  auto escaped_text = escape_for_c_string(text);
  os << "SHIFT (" << at(grammar->symbol_names, token) << ")["
    << escaped_text << "]\n";
  return escaped_text;
}

std::any debug_reader::at_reduce(int prod_i, std::vector<std::any>& rhs) {
  os << "REDUCE";
  std::string lhs_text;
  auto& prod = at(grammar->productions, prod_i);
  for (int i = 0; i < size(prod.rhs); ++i) {
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
