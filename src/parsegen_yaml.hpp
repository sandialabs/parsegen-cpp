#ifndef PARSEGEN_YAML_HPP
#define PARSEGEN_YAML_HPP

#include "parsegen_language.hpp"
#include "parsegen_parser_tables.hpp"
#include "parsegen_parser.hpp"

namespace parsegen {
namespace yaml {

enum {
  PROD_DOC,
  PROD_DOC2,
  PROD_TOP_FIRST,
  PROD_TOP_NEXT,
  PROD_TOP_DIRECT,
  PROD_TOP_BEGIN,
  PROD_TOP_END,
  PROD_TOP_BMAP,
  PROD_BMAP_FIRST,
  PROD_BMAP_NEXT,
  PROD_BMAP_SCALAR,
  PROD_BMAP_BSCALAR,
  PROD_BMAP_BVALUE,
  PROD_BVALUE_EMPTY,
  PROD_BVALUE_BMAP,
  PROD_BVALUE_BSEQ,
  PROD_BMAP_FMAP,
  PROD_BMAP_FSEQ,
  PROD_BSEQ_FIRST,
  PROD_BSEQ_NEXT,
  PROD_BSEQ_SCALAR,
  PROD_BSEQ_BSCALAR,
  PROD_BSEQ_BMAP,
  PROD_BSEQ_BMAP_TRAIL,
  PROD_BSEQ_BSEQ,
  PROD_BSEQ_BSEQ_TRAIL,
  PROD_BSEQ_FMAP,
  PROD_BSEQ_FSEQ,
  PROD_FMAP,
  PROD_FMAP_EMPTY,
  PROD_FMAP_FIRST,
  PROD_FMAP_NEXT,
  PROD_FMAP_SCALAR,
  PROD_FMAP_FMAP,
  PROD_FMAP_FSEQ,
  PROD_FSEQ,
  PROD_FSEQ_EMPTY,
  PROD_FSEQ_FIRST,
  PROD_FSEQ_NEXT,
  PROD_FSEQ_SCALAR,
  PROD_FSEQ_FMAP,
  PROD_FSEQ_FSEQ,
  PROD_SCALAR_RAW,
  PROD_SCALAR_QUOTED,
  PROD_MAP_SCALAR_RAW,
  PROD_MAP_SCALAR_QUOTED,
  PROD_SCALAR_DQUOTED,
  PROD_SCALAR_SQUOTED,
  PROD_SCALAR_HEAD_OTHER,
  PROD_SCALAR_HEAD_DOT,
  PROD_SCALAR_HEAD_DASH,
  PROD_SCALAR_HEAD_DOT_DOT,
  PROD_MAP_SCALAR_ESCAPED_EMPTY,
  PROD_MAP_SCALAR_ESCAPED_NEXT,
  PROD_TAG_EMPTY,
  PROD_TAG,
  PROD_BSCALAR,
  PROD_BSCALAR_FIRST,
  PROD_BSCALAR_NEXT,
  PROD_BSCALAR_LINE,
  PROD_BSCALAR_INDENT,
  PROD_BSCALAR_HEADER_LITERAL,
  PROD_BSCALAR_HEADER_FOLDED,
  PROD_BSCALAR_HEAD_EMPTY,
  PROD_BSCALAR_HEAD_NEXT,
  PROD_BSCALAR_HEAD_OTHER,
  PROD_BSCALAR_HEAD_DASH,
  PROD_DQUOTED_EMPTY,
  PROD_DQUOTED_NEXT,
  PROD_SQUOTED_EMPTY,
  PROD_SQUOTED_NEXT,
  PROD_ANY_EMPTY,
  PROD_ANY_NEXT,
  PROD_DESCAPE_EMPTY,
  PROD_DESCAPE_NEXT,
  PROD_DESCAPE,
  PROD_SESCAPE_EMPTY,
  PROD_SESCAPE_NEXT,
  PROD_SESCAPE,
  PROD_SCALAR_TAIL_EMPTY,
  PROD_SCALAR_TAIL_NEXT,
  PROD_OTHER_FIRST,
  PROD_OTHER_NEXT,
  PROD_SCALAR_TAIL_SPACE,
  PROD_SCALAR_TAIL_DOT,
  PROD_SCALAR_TAIL_DASH,
  PROD_SCALAR_TAIL_SQUOT,
  PROD_SCALAR_TAIL_OTHER,
  PROD_DESCAPED_DQUOT,
  PROD_DESCAPED_SLASH,
  PROD_DESCAPED_DQUOTED,
  PROD_DQUOTED_COMMON,
  PROD_DQUOTED_SQUOT,
  PROD_SQUOTED_COMMON,
  PROD_SQUOTED_DQUOT,
  PROD_SQUOTED_SLASH,
  PROD_ANY_COMMON,
  PROD_ANY_DQUOT,
  PROD_ANY_SQUOT,
  PROD_ANY_SLASH,
  PROD_COMMON_SPACE,
  PROD_COMMON_COLON,
  PROD_COMMON_DOT,
  PROD_COMMON_DASH,
  PROD_COMMON_PIPE,
  PROD_COMMON_LSQUARE,
  PROD_COMMON_RSQUARE,
  PROD_COMMON_LCURLY,
  PROD_COMMON_RCURLY,
  PROD_COMMON_RANGLE,
  PROD_COMMON_COMMA,
  PROD_COMMON_PERCENT,
  PROD_COMMON_EXCL,
  PROD_COMMON_OTHER,
  PROD_SPACE_STAR_EMPTY,
  PROD_SPACE_STAR_NEXT,
  PROD_SPACE_PLUS_FIRST,
  PROD_SPACE_PLUS_NEXT
};

enum { NPRODS = PROD_SPACE_PLUS_NEXT + 1 };

enum {
  TOK_NEWLINE,
  TOK_INDENT,
  TOK_DEDENT,
  TOK_SPACE,
  TOK_COLON,
  TOK_DOT,
  TOK_DASH,
  TOK_DQUOT,
  TOK_SQUOT,
  TOK_SLASH,
  TOK_PIPE,
  TOK_LSQUARE,
  TOK_RSQUARE,
  TOK_LCURLY,
  TOK_RCURLY,
  TOK_RANGLE,
  TOK_COMMA,
  TOK_PERCENT,
  TOK_EXCL,
  TOK_OTHER
};

enum { NTOKS = TOK_OTHER + 1 };

language build_language();
language_ptr ask_language();
parser_tables_ptr ask_parser_tables();

class object;
class scalar;
class map;
class sequence;

class object {
 public:
  virtual ~object() = default;
  bool is_scalar() const;
  bool is_map() const;
  bool is_sequence() const;
  scalar const& as_scalar() const;
  map const& as_map() const;
  sequence const& as_sequence() const;
  virtual void print(std::ostream& s, std::string const& indent = "") const = 0;
};

class scalar : public object {
  std::string m_value;
 public:
  scalar(std::string&& string_arg);
  scalar(std::string const& string_arg);
  bool operator<(scalar const& other) const;
  std::string const& string() const { return m_value; }
  void print(std::ostream& s, std::string const& indent = "") const override;
};

class map : public object {
 public:
  using item = std::pair<scalar, std::shared_ptr<object>>;
  using impl_t = std::map<scalar, std::shared_ptr<object>>;
  using const_iterator = impl_t::const_iterator;
 private:
  std::map<scalar, std::shared_ptr<object>> m_impl;
 public:
  void insert(item&& item_arg);
  bool has(std::string const& key) const;
  object const& operator[](std::string const& key) const;
  const_iterator begin() const;
  const_iterator end() const;
  void print(std::ostream& s, std::string const& indent = "") const override;
};

class sequence : public object {
 public:
  using impl_t = std::vector<std::shared_ptr<object>>;
  using const_iterator = impl_t::const_iterator;
 private:
  impl_t m_impl;
 public:
  void append(std::shared_ptr<object>&& item);
  const_iterator begin() const;
  const_iterator end() const;
  int size() const;
  object const& operator[](int i) const;
  void print(std::ostream& s, std::string const& indent = "") const override;
};

class parser_impl : public parsegen::parser {
 public:
  parser_impl();
  std::any shift(int token, std::string& text) override;
  std::any reduce(
      int production,
      std::vector<std::any>& rhs) override;
};

class parser {
  parser_impl m_impl;
 public:
  map parse_stream(
      std::istream& stream,
      std::string const& stream_name_in = "");
  map parse_string(
      std::string const& string,
      std::string const& string_name = "");
  map parse_file(
      std::filesystem::path const& file_path);
};

}  // end namespace yaml
}  // end namespace parsegen

#endif
