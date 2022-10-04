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
  std::string m_user_message;
  std::string m_parser_message;
  std::string m_full_message;
 public:
  error(std::string const& user_message_arg, std::string const& parser_message_arg = "")
    :std::runtime_error("")
    ,m_user_message(user_message_arg)
    ,m_parser_message(parser_message_arg)
  {
    m_full_message = m_user_message + m_parser_message;
  }
  void set_user_message(std::string const& user_message_arg)
  {
    m_user_message = user_message_arg;
    m_full_message = m_user_message + m_parser_message;
  }
  void set_parser_message(std::string const& parser_message_arg)
  {
    m_parser_message = parser_message_arg;
    m_full_message = m_user_message + m_parser_message;
  }
  virtual const char* what() const noexcept override
  {
    return m_full_message.c_str();
  }
};

class unacceptable_token : public error {
 public:
  unacceptable_token(std::string const& parser_message_arg)
    :error("", parser_message_arg)
  {}
};

class bad_character : public error {
 public:
  bad_character(std::string const& parser_message_arg)
    :error("", parser_message_arg)
  {}
};

class tokenization_failure : public error {
 public:
  tokenization_failure(std::string const& parser_message_arg)
    :error("", parser_message_arg)
  {}
};

}
