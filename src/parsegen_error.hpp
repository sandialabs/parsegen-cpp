#pragma once

#include <stdexcept>
#include <string>

namespace parsegen {

class parse_error : public std::invalid_argument {
 public:
  parse_error(const std::string& msg);
  virtual void out_of_line_virtual_method();
};

class error : public std::runtime_error {
  std::string m_before_message;
  std::string m_parser_message;
  std::string m_after_message;
  std::string m_full_message;
 public:
  error(
      std::string const& before_message_arg,
      std::string const& after_message_arg = "",
      std::string const& parser_message_arg = "")
    :std::runtime_error("")
    ,m_before_message(before_message_arg)
    ,m_parser_message(parser_message_arg)
    ,m_after_message(after_message_arg)
  {
    m_full_message = m_before_message + m_parser_message + m_after_message;
  }
  void set_user_message(std::string const& before_message_arg)
  {
    m_before_message = before_message_arg;
    m_full_message = m_before_message + m_parser_message + m_after_message;
  }
  void set_after_message(std::string const& after_message_arg)
  {
    m_after_message = after_message_arg;
    m_full_message = m_before_message + m_parser_message + m_after_message;
  }
  void set_parser_message(std::string const& parser_message_arg)
  {
    m_parser_message = parser_message_arg;
    m_full_message = m_before_message + m_parser_message + m_after_message;
  }
  virtual const char* what() const noexcept override
  {
    return m_full_message.c_str();
  }
};

class unacceptable_token : public error {
  std::string m_token_name;
 public:
  unacceptable_token(
      std::string const& parser_message_arg,
      std::string const& token_name_arg)
    :error("Unacceptable token\n", "", parser_message_arg)
    ,m_token_name(token_name_arg)
  {
  }
  std::string const& token_name() const { return m_token_name; }
};

class bad_character : public error {
 public:
  bad_character(std::string const& parser_message_arg)
    :error(
      "Encountered a non-ASCII character\n",
      "This parser can only handle ASCII characters.\n"
      "Usually, non-ASCII characters are caused by trying to "
      "use foreign-language accents or by copying text from a web page.\n",
      parser_message_arg)
  {}
};

class tokenization_failure : public error {
 public:
  tokenization_failure(std::string const& parser_message_arg)
    :error("Could not tokenize the text\n", "", parser_message_arg)
  {}
};

}
