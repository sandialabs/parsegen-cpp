#include "parsegen_yaml.hpp"

namespace parsegen {
namespace yaml {

static void build_productions(std::vector<language::production>& prods) {
  prods.resize(NPRODS);
  prods[PROD_DOC] = {"doc", {"top_items"}};
  prods[PROD_DOC2] = {"doc", {"NEWLINE", "top_items"}};
  prods[PROD_TOP_FIRST] = {"top_items", {"top_item"}};
  prods[PROD_TOP_NEXT] = {"top_items", {"top_items", "top_item"}};
  prods[PROD_TOP_DIRECT] = {"top_item", {"%", "any*", "NEWLINE"}};
  prods[PROD_TOP_BEGIN] = {"top_item", {"-", "-", "-", "NEWLINE"}};
  prods[PROD_TOP_END] = {"top_item", {".", ".", ".", "NEWLINE"}};
  prods[PROD_TOP_BMAP] = {"top_item", {"bmap_item"}};
  prods[PROD_BMAP_FIRST] = {"bmap_items", {"bmap_item"}};
  prods[PROD_BMAP_NEXT] = {"bmap_items", {"bmap_items", "bmap_item"}};
  prods[PROD_BMAP_SCALAR] = {
      "bmap_item", {"scalar", ":", "WS*", "tag?", "map_scalar", "NEWLINE"}};
  prods[PROD_BMAP_BSCALAR] = {"bmap_item", {"scalar", ":", "WS*", "bscalar"}};
  prods[PROD_BMAP_BVALUE] = {
      "bmap_item", {"scalar", ":", "WS*", "NEWLINE", "bvalue"}};
  prods[PROD_BVALUE_EMPTY] = {"bvalue", {}};
  prods[PROD_BVALUE_BMAP] = {"bvalue", {"INDENT", "bmap_items", "DEDENT"}};
  /* TODO: allow a tag in this */
  prods[PROD_BVALUE_BSEQ] = {"bvalue", {"INDENT", "bseq_items", "DEDENT"}};
  prods[PROD_BMAP_FMAP] = {
      "bmap_item", {"scalar", ":", "WS*", "tag?", "fmap", "NEWLINE"}};
  prods[PROD_BMAP_FSEQ] = {
      "bmap_item", {"scalar", ":", "WS*", "tag?", "fseq", "NEWLINE"}};
  prods[PROD_BSEQ_FIRST] = {"bseq_items", {"bseq_item"}};
  prods[PROD_BSEQ_NEXT] = {"bseq_items", {"bseq_items", "bseq_item"}};
  prods[PROD_BSEQ_SCALAR] = {
      "bseq_item", {"-", "WS+", "tag?", "scalar", "NEWLINE"}};
  prods[PROD_BSEQ_BSCALAR] = {"bseq_item", {"-", "WS+", "bscalar"}};
  prods[PROD_BSEQ_BMAP] = {
      "bseq_item", {"-", "NEWLINE", "INDENT", "bmap_items", "DEDENT"}};
  prods[PROD_BSEQ_BMAP_TRAIL] = {
      "bseq_item", {"-", "WS+", "NEWLINE", "INDENT", "bmap_items", "DEDENT"}};
  prods[PROD_BSEQ_BSEQ] = {
      "bseq_item", {"-", "NEWLINE", "INDENT", "bseq_items", "DEDENT"}};
  prods[PROD_BSEQ_BSEQ_TRAIL] = {
      "bseq_item", {"-", "WS+", "NEWLINE", "INDENT", "bseq_items", "DEDENT"}};
  prods[PROD_BSEQ_FMAP] = {
      "bseq_item", {"-", "WS+", "tag?", "fmap", "NEWLINE"}};
  prods[PROD_BSEQ_FSEQ] = {
      "bseq_item", {"-", "WS+", "tag?", "fseq", "NEWLINE"}};
  prods[PROD_FMAP] = {"fmap", {"{", "WS*", "fmap_items", "}", "WS*"}};
  prods[PROD_FMAP_EMPTY] = {"fmap", {"{", "WS*", "}", "WS*"}};
  prods[PROD_FMAP_FIRST] = {"fmap_items", {"fmap_item"}};
  prods[PROD_FMAP_NEXT] = {
      "fmap_items", {"fmap_items", ",", "WS*", "fmap_item"}};
  prods[PROD_FMAP_SCALAR] = {
      "fmap_item", {"scalar", ":", "WS*", "tag?", "scalar"}};
  prods[PROD_FMAP_FMAP] = {"fmap_item", {"scalar", ":", "WS*", "tag?", "fmap"}};
  prods[PROD_FMAP_FSEQ] = {"fmap_item", {"scalar", ":", "WS*", "tag?", "fseq"}};
  prods[PROD_FSEQ] = {"fseq", {"[", "WS*", "fseq_items", "]", "WS*"}};
  prods[PROD_FSEQ_EMPTY] = {"fseq", {"[", "WS*", "]", "WS*"}};
  prods[PROD_FSEQ_FIRST] = {"fseq_items", {"fseq_item"}};
  prods[PROD_FSEQ_NEXT] = {
      "fseq_items", {"fseq_items", ",", "WS*", "fseq_item"}};
  prods[PROD_FSEQ_SCALAR] = {"fseq_item", {"tag?", "scalar"}};
  prods[PROD_FSEQ_FMAP] = {"fseq_item", {"tag?", "fmap"}};
  prods[PROD_FSEQ_FSEQ] = {"fseq_item", {"tag?", "fseq"}};
  prods[PROD_SCALAR_RAW] = {"scalar", {"scalar_head", "scalar_tail*"}};
  prods[PROD_SCALAR_QUOTED] = {"scalar", {"scalar_quoted"}};
  prods[PROD_MAP_SCALAR_RAW] = {
      "map_scalar", {"scalar_head", "scalar_tail*", "map_scalar_escaped*"}};
  prods[PROD_MAP_SCALAR_QUOTED] = {"map_scalar", {"scalar_quoted"}};
  prods[PROD_SCALAR_DQUOTED] = {
      "scalar_quoted", {"\"", "dquoted*", "descape*", "\"", "WS*"}};
  prods[PROD_SCALAR_SQUOTED] = {
      "scalar_quoted", {"'", "squoted*", "sescape*", "'", "WS*"}};
  prods[PROD_SCALAR_HEAD_OTHER] = {"scalar_head", {"OTHERCHAR"}};
  prods[PROD_SCALAR_HEAD_DOT] = {"scalar_head", {".", "OTHERCHAR"}};
  prods[PROD_SCALAR_HEAD_DASH] = {"scalar_head", {"-", "OTHERCHAR"}};
  prods[PROD_SCALAR_HEAD_DOT_DOT] = {"scalar_head", {".", ".", "OTHERCHAR"}};
  prods[PROD_MAP_SCALAR_ESCAPED_EMPTY] = {"map_scalar_escaped*", {}};
  prods[PROD_MAP_SCALAR_ESCAPED_NEXT] = {
      "map_scalar_escaped*", {"map_scalar_escaped*", ",", "scalar_tail*"}};
  prods[PROD_TAG_EMPTY] = {"tag?", {}};
  prods[PROD_TAG] = {"tag?", {"!", "!", "OTHERCHAR+", "WS+"}};
  prods[PROD_BSCALAR] = {"bscalar", {"bscalar_header", "WS*", "NEWLINE",
                                        "INDENT", "bscalar_items", "DEDENT"}};
  prods[PROD_BSCALAR_FIRST] = {"bscalar_items", {"bscalar_item"}};
  prods[PROD_BSCALAR_NEXT] = {
      "bscalar_items", {"bscalar_items", "bscalar_item"}};
  prods[PROD_BSCALAR_LINE] = {"bscalar_item", {"any*", "NEWLINE"}};
  prods[PROD_BSCALAR_INDENT] = {
      "bscalar_item", {"INDENT", "bscalar_items", "DEDENT"}};
  prods[PROD_BSCALAR_HEADER_LITERAL] = {
      "bscalar_header", {"|", "bscalar_head_c*"}};
  prods[PROD_BSCALAR_HEADER_FOLDED] = {
      "bscalar_header", {">", "bscalar_head_c*"}};
  prods[PROD_BSCALAR_HEAD_EMPTY] = {"bscalar_head_c*", {}};
  prods[PROD_BSCALAR_HEAD_NEXT] = {
      "bscalar_head_c*", {"bscalar_head_c*", "bscalar_head_c"}};
  prods[PROD_BSCALAR_HEAD_OTHER] = {"bscalar_head_c", {"OTHERCHAR"}};
  prods[PROD_BSCALAR_HEAD_DASH] = {"bscalar_head_c", {"-"}};
  prods[PROD_DQUOTED_EMPTY] = {"dquoted*", {}};
  prods[PROD_DQUOTED_NEXT] = {"dquoted*", {"dquoted*", "dquoted"}};
  prods[PROD_SQUOTED_EMPTY] = {"squoted*", {}};
  prods[PROD_SQUOTED_NEXT] = {"squoted*", {"squoted*", "squoted"}};
  prods[PROD_ANY_EMPTY] = {"any*", {}};
  prods[PROD_ANY_NEXT] = {"any*", {"any*", "any"}};
  prods[PROD_DESCAPE_EMPTY] = {"descape*", {}};
  prods[PROD_DESCAPE_NEXT] = {"descape*", {"descape*", "descape"}};
  prods[PROD_DESCAPE] = {"descape", {"\\", "descaped", "dquoted*"}};
  prods[PROD_SESCAPE_EMPTY] = {"sescape*", {}};
  prods[PROD_SESCAPE_NEXT] = {"sescape*", {"sescape*", "sescape"}};
  prods[PROD_SESCAPE] = {"sescape", {"'", "'", "squoted*"}};
  prods[PROD_SCALAR_TAIL_EMPTY] = {"scalar_tail*", {}};
  prods[PROD_SCALAR_TAIL_NEXT] = {
      "scalar_tail*", {"scalar_tail*", "scalar_tail"}};
  prods[PROD_OTHER_FIRST] = {"OTHERCHAR+", {"OTHERCHAR"}};
  prods[PROD_OTHER_NEXT] = {"OTHERCHAR+", {"OTHERCHAR+", "OTHERCHAR"}};
  prods[PROD_SCALAR_TAIL_SPACE] = {"scalar_tail", {"WS"}};
  prods[PROD_SCALAR_TAIL_DOT] = {"scalar_tail", {"."}};
  prods[PROD_SCALAR_TAIL_DASH] = {"scalar_tail", {"-"}};
  prods[PROD_SCALAR_TAIL_SQUOT] = {"scalar_tail", {"'"}};
  prods[PROD_SCALAR_TAIL_OTHER] = {"scalar_tail", {"OTHERCHAR"}};
  prods[PROD_DESCAPED_DQUOT] = {"descaped", {"\""}};
  prods[PROD_DESCAPED_SLASH] = {"descaped", {"\\"}};
  prods[PROD_DESCAPED_DQUOTED] = {"descaped", {"dquoted"}};
  prods[PROD_DQUOTED_COMMON] = {"dquoted", {"common"}};
  prods[PROD_DQUOTED_SQUOT] = {"dquoted", {"'"}};
  prods[PROD_SQUOTED_COMMON] = {"squoted", {"common"}};
  prods[PROD_SQUOTED_DQUOT] = {"squoted", {"\""}};
  prods[PROD_SQUOTED_SLASH] = {"squoted", {"\\"}};
  prods[PROD_ANY_COMMON] = {"any", {"common"}};
  prods[PROD_ANY_DQUOT] = {"any", {"\""}};
  prods[PROD_ANY_SQUOT] = {"any", {"'"}};
  prods[PROD_ANY_SLASH] = {"any", {"\\"}};
  prods[PROD_COMMON_SPACE] = {"common", {"WS"}};
  prods[PROD_COMMON_COLON] = {"common", {":"}};
  prods[PROD_COMMON_DOT] = {"common", {"."}};
  prods[PROD_COMMON_DASH] = {"common", {"-"}};
  prods[PROD_COMMON_PIPE] = {"common", {"|"}};
  prods[PROD_COMMON_LSQUARE] = {"common", {"["}};
  prods[PROD_COMMON_RSQUARE] = {"common", {"]"}};
  prods[PROD_COMMON_LCURLY] = {"common", {"{"}};
  prods[PROD_COMMON_RCURLY] = {"common", {"}"}};
  prods[PROD_COMMON_RANGLE] = {"common", {">"}};
  prods[PROD_COMMON_COMMA] = {"common", {","}};
  prods[PROD_COMMON_PERCENT] = {"common", {"%"}};
  prods[PROD_COMMON_EXCL] = {"common", {"!"}};
  prods[PROD_COMMON_OTHER] = {"common", {"OTHERCHAR"}};
  prods[PROD_SPACE_STAR_EMPTY] = {"WS*", {}};
  prods[PROD_SPACE_STAR_NEXT] = {"WS*", {"WS*", "WS"}};
  prods[PROD_SPACE_PLUS_FIRST] = {"WS+", {"WS"}};
  prods[PROD_SPACE_PLUS_NEXT] = {"WS+", {"WS+", "WS"}};
}

language build_language() {
  language out;
  auto& prods = out.productions;
  auto& toks = out.tokens;
  build_productions(prods);
  toks.resize(NTOKS);
  toks[TOK_NEWLINE] = {"NEWLINE", "((#[^\r\n]*)?\r?\n[ \t]*)+"};
  toks[TOK_INDENT] = {"INDENT", "((#[^\r\n]*)?\r?\n[ \t]*)+"};
  toks[TOK_DEDENT] = {"DEDENT", "((#[^\r\n]*)?\r?\n[ \t]*)+"};
  toks[TOK_SPACE] = {"WS", "[ \t]"};
  toks[TOK_COLON] = {":", ":"};
  toks[TOK_DOT] = {".", "\\."};
  toks[TOK_DASH] = {"-", "\\-"};
  toks[TOK_DQUOT] = {"\"", "\""};
  toks[TOK_SQUOT] = {"'", "'"};
  toks[TOK_SLASH] = {"\\", "\\\\"};
  toks[TOK_PIPE] = {"|", "\\|"};
  toks[TOK_LSQUARE] = {"[", "\\["};
  toks[TOK_RSQUARE] = {"]", "\\]"};
  toks[TOK_LCURLY] = {"{", "{"};
  toks[TOK_RCURLY] = {"}", "}"};
  toks[TOK_RANGLE] = {">", ">"};
  toks[TOK_COMMA] = {",", ","};
  toks[TOK_PERCENT] = {"%", "%"};
  toks[TOK_EXCL] = {"!", "!"};
  toks[TOK_OTHER] = {"OTHERCHAR", "[^ \t:\\.\\-\"'\\\\\\|\\[\\]{}>,%#!\n\r]"};
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

reader_tables_ptr ask_reader_tables() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
  static reader_tables_ptr ptr;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  if (ptr.use_count() == 0) {
    ptr = build_reader_tables(*(yaml::ask_language()));
  }
  return ptr;
}

bool object::is_scalar() const
{
  return dynamic_cast<scalar const*>(this) != nullptr;
}

bool object::is_map() const
{
  return dynamic_cast<map const*>(this) != nullptr;
}

bool object::is_sequence() const
{
  return dynamic_cast<sequence const*>(this) != nullptr;
}

scalar const& object::as_scalar() const
{
  return dynamic_cast<scalar const&>(*this);
}

map const& object::as_map() const
{
  return dynamic_cast<map const&>(*this);
}

sequence const& object::as_sequence() const
{
  return dynamic_cast<sequence const&>(*this);
}

scalar::scalar(std::string&& string_arg)
  :m_value(std::move(string_arg))
{
}

scalar::scalar(std::string const& string_arg)
  :m_value(string_arg)
{
}

bool scalar::operator<(scalar const& other) const
{
  return m_value < other.m_value;
}

void scalar::print(std::ostream& s, std::string const&) const
{
  s << m_value;
}

void map::insert(item&& item_arg)
{
  m_impl.insert(std::move(item_arg));
}

object const& map::operator[](
    std::string const& key) const
{
  scalar scalar_key(key);
  auto it = m_impl.find(scalar_key);
  if (it == m_impl.end()) {
    throw std::invalid_argument(
        "yaml::map key not found: " + key);
  }
  return *(it->second);
}

map::const_iterator map::begin() const
{
  return m_impl.begin();
}

map::const_iterator map::end() const
{
  return m_impl.end();
}

void map::print(std::ostream& s, std::string const& indent) const
{
  for (map::item const& item : *this) {
    scalar const& scalar_key = item.first;
    object const& value = *(item.second);
    s << indent;
    scalar_key.print(s);
    if (value.is_scalar()) {
      scalar const& scalar_value = value.as_scalar();
      std::string const& string_value = scalar_value.string();
      s << ": " << string_value << '\n';
    } else if (value.is_map()) {
      s << ": \n";
      value.as_map().print(s, indent + "  ");
    } else if (value.is_sequence()) {
      s << ": \n";
      value.as_sequence().print(s, indent + "  ");
    }
  }
}

void sequence::append(
    std::shared_ptr<object>&& item)
{
  m_impl.push_back(std::move(item));
}

sequence::const_iterator sequence::begin() const
{
  return m_impl.begin();
}

sequence::const_iterator sequence::end() const
{
  return m_impl.end();
}

int sequence::size() const
{
  return int(m_impl.size());
}

object const& sequence::operator[](int i) const
{
  return *(m_impl[std::size_t(i)]);
}

void sequence::print(std::ostream& s, std::string const& indent) const
{
  for (std::shared_ptr<parsegen::yaml::object> const& obj_ptr : *this) {
    parsegen::yaml::object const& item = *obj_ptr;
    s << indent << "- ";
    if (item.is_scalar()) {
      parsegen::yaml::scalar const& scalar_value = item.as_scalar();
      scalar_value.print(s);
      s << '\n';
    } else if (item.is_map()) {
      s << '\n';
      item.as_map().print(s, indent + "  ");
    } else if (item.is_sequence()) {
      s << '\n';
      item.as_sequence().print(s, indent + "  ");
    }
  }
}

reader_impl::reader_impl()
  :parsegen::reader(ask_reader_tables())
{}

std::any reader_impl::at_shift(
    int token, std::string& text)
{
  switch (token) {
    case TOK_OTHER:
    case TOK_SPACE:
      return text[0];
  }
  return std::any();
}

std::any reader_impl::at_reduce(
    int production,
    std::vector<std::any>& rhs)
{
  switch (production) {
    case PROD_DOC:
    case PROD_TOP_BMAP: {
      return std::move(rhs.at(0));
    }
    case PROD_DOC2: {
      return std::move(rhs.at(1));
    }
    case PROD_TOP_FIRST: {
      map result;
      if (rhs.at(0).type() == typeid(map::item)) {
        map::item& item = std::any_cast<map::item&>(
            rhs.at(0));
        result.insert(std::move(item));
      }
      return result;
    }
    case PROD_TOP_NEXT: {
      map& result = std::any_cast<map&>(rhs.at(0));
      if (rhs.at(1).type() == typeid(map::item)) {
        map::item& item = std::any_cast<map::item&>(
            rhs.at(1));
        result.insert(std::move(item));
      }
      return std::move(result);
    }
    case PROD_BMAP_FIRST: {
      map result;
      map::item& item = std::any_cast<map::item&>(
          rhs.at(0));
      result.insert(std::move(item));
      return result;
    }
    case PROD_BMAP_NEXT: {
      map& result = std::any_cast<map&>(rhs.at(0));
      map::item& item = std::any_cast<map::item&>(
          rhs.at(1));
      result.insert(std::move(item));
      return std::move(result);
    }
    case PROD_BMAP_SCALAR: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      scalar& scalar_value =
        std::any_cast<scalar&>(rhs.at(4));
      std::shared_ptr<object> value(
          new scalar(std::move(scalar_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_BMAP_BSCALAR: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      scalar& scalar_value =
        std::any_cast<scalar&>(rhs.at(3));
      std::shared_ptr<object> value(
          new scalar(std::move(scalar_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_BMAP_BVALUE: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      std::shared_ptr<object>& value =
        std::any_cast<std::shared_ptr<object>&>(rhs.at(4));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_BVALUE_BMAP: {
      map& map_value = std::any_cast<map&>(rhs.at(1));
      std::shared_ptr<object> value(
          new map(std::move(map_value)));
      return value;
    }
    case PROD_BVALUE_BSEQ: {
      sequence& sequence_value =
        std::any_cast<sequence&>(rhs.at(1));
      std::shared_ptr<object> value(
          new sequence(std::move(sequence_value)));
      return value;
    }
    case PROD_BMAP_FMAP: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      map& map_value =
        std::any_cast<map&>(rhs.at(4));
      std::shared_ptr<object> value(
          new map(std::move(map_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_BMAP_FSEQ: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      sequence& sequence_value =
        std::any_cast<sequence&>(rhs.at(4));
      std::shared_ptr<object> value(
          new sequence(std::move(sequence_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_BSEQ_FIRST: {
      sequence result;
      std::shared_ptr<object>& value =
        std::any_cast<std::shared_ptr<object>&>(rhs.at(0));
      result.append(std::move(value));
      return result;
    }
    case PROD_BSEQ_NEXT: {
      sequence& result =
        std::any_cast<sequence&>(rhs.at(0));
      std::shared_ptr<object>& value =
        std::any_cast<std::shared_ptr<object>&>(rhs.at(1));
      result.append(std::move(value));
      return std::move(result);
    }
    case PROD_BSEQ_SCALAR: {
      scalar& scalar_value =
        std::any_cast<scalar&>(rhs.at(3));
      std::shared_ptr<object> value(
          new scalar(std::move(scalar_value)));
      return value;
    }
    case PROD_BSEQ_BSCALAR: {
      scalar& scalar_value =
        std::any_cast<scalar&>(rhs.at(2));
      std::shared_ptr<object> value(
          new scalar(std::move(scalar_value)));
      return value;
    }
    case PROD_BSEQ_BMAP:
    case PROD_BSEQ_FMAP: {
      map& map_value =
        std::any_cast<map&>(rhs.at(3));
      std::shared_ptr<object> value(
          new map(std::move(map_value)));
      return value;
    }
    case PROD_BSEQ_BMAP_TRAIL: {
      map& map_value =
        std::any_cast<map&>(rhs.at(4));
      std::shared_ptr<object> value(
          new map(std::move(map_value)));
      return value;
    }
    case PROD_BSEQ_BSEQ:
    case PROD_BSEQ_FSEQ: {
      sequence& sequence_value =
        std::any_cast<sequence&>(rhs.at(3));
      std::shared_ptr<object> value(
          new sequence(std::move(sequence_value)));
      return value;
    }
    case PROD_BSEQ_BSEQ_TRAIL: {
      sequence& sequence_value =
        std::any_cast<sequence&>(rhs.at(4));
      std::shared_ptr<object> value(
          new sequence(std::move(sequence_value)));
      return value;
    }
    case PROD_FMAP:
    case PROD_FSEQ: {
      return std::move(rhs.at(2));
    }
    case PROD_FMAP_EMPTY: {
      return map();
    }
    case PROD_FMAP_FIRST: {
      map::item& item = std::any_cast<map::item&>(rhs.at(0));
      map result;
      result.insert(std::move(item));
      return result;
    }
    case PROD_FMAP_NEXT: {
      map& result = std::any_cast<map&>(rhs.at(0));
      map::item& item =
        std::any_cast<map::item&>(rhs.at(3));
      result.insert(std::move(item));
      return std::move(result);
    }
    case PROD_FMAP_SCALAR: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      scalar& scalar_value =
        std::any_cast<scalar&>(rhs.at(4));
      std::shared_ptr<object> value(
          new scalar(std::move(scalar_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_FMAP_FMAP: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      map& map_value =
        std::any_cast<map&>(rhs.at(4));
      std::shared_ptr<object> value(
          new map(std::move(map_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_FMAP_FSEQ: {
      scalar& key = std::any_cast<scalar&>(rhs.at(0));
      sequence& sequence_value =
        std::any_cast<sequence&>(rhs.at(4));
      std::shared_ptr<object> value(
          new sequence(std::move(sequence_value)));
      return map::item(
          std::move(key), std::move(value));
    }
    case PROD_FSEQ_EMPTY: {
      return sequence();
    }
    case PROD_FSEQ_FIRST: {
      std::shared_ptr<object>& value =
        std::any_cast<std::shared_ptr<object>&>(rhs.at(0));
      sequence result;
      result.append(std::move(value));
      return result;
    }
    case PROD_FSEQ_NEXT: {
      sequence& result =
        std::any_cast<sequence&>(rhs.at(0));
      std::shared_ptr<object>& value =
        std::any_cast<std::shared_ptr<object>&>(rhs.at(3));
      result.append(std::move(value));
      return std::move(result);
    }
    case PROD_FSEQ_SCALAR: {
      scalar& scalar_value =
        std::any_cast<scalar&>(rhs.at(1));
      std::shared_ptr<object> value(
          new scalar(std::move(scalar_value)));
      return value;
    }
    case PROD_FSEQ_FMAP: {
      map& map_value =
        std::any_cast<map&>(rhs.at(1));
      std::shared_ptr<object> value(
          new map(std::move(map_value)));
      return value;
    }
    case PROD_FSEQ_FSEQ: {
      sequence& sequence_value =
        std::any_cast<sequence&>(rhs.at(1));
      std::shared_ptr<object> value(
          new sequence(std::move(sequence_value)));
      return value;
    }
    case PROD_SCALAR_RAW: {
      std::string& head =
        std::any_cast<std::string&>(rhs.at(0));
      std::string& tail_star =
        std::any_cast<std::string&>(rhs.at(1));
      std::string scalar_string(std::move(head));
      scalar_string += tail_star;
      return scalar(scalar_string);
    }
    case PROD_SCALAR_QUOTED:
    case PROD_MAP_SCALAR_QUOTED: {
      return std::move(rhs.at(0));
    }
    case PROD_MAP_SCALAR_RAW: {
      std::string& head =
        std::any_cast<std::string&>(rhs.at(0));
      std::string& tail_star =
        std::any_cast<std::string&>(rhs.at(1));
      std::string& map_escaped_star =
        std::any_cast<std::string&>(rhs.at(2));
      std::string scalar_string(std::move(head));
      scalar_string += tail_star;
      scalar_string += map_escaped_star;
      return scalar(scalar_string);
    }
    case PROD_SCALAR_DQUOTED:
    case PROD_SCALAR_SQUOTED: {
      std::string& quoted_star =
        std::any_cast<std::string&>(rhs.at(1));
      std::string& escaped_star =
        std::any_cast<std::string&>(rhs.at(2));
      std::string scalar_string(std::move(quoted_star));
      scalar_string += escaped_star;
      return scalar(scalar_string);
    }
    case PROD_SCALAR_HEAD_OTHER: {
      char otherchar = std::any_cast<char>(rhs.at(0));
      std::string head;
      head.push_back(otherchar);
      return head;
    }
    case PROD_SCALAR_HEAD_DOT: {
      char otherchar = std::any_cast<char>(rhs.at(1));
      std::string head;
      head.push_back('.');
      head.push_back(otherchar);
      return head;
    }
    case PROD_SCALAR_HEAD_DASH: {
      char otherchar = std::any_cast<char>(rhs.at(1));
      std::string head;
      head.push_back('-');
      head.push_back(otherchar);
      return head;
    }
    case PROD_SCALAR_HEAD_DOT_DOT: {
      char otherchar = std::any_cast<char>(rhs.at(2));
      std::string head;
      head.push_back('.');
      head.push_back('.');
      head.push_back(otherchar);
      return head;
    }
    case PROD_MAP_SCALAR_ESCAPED_EMPTY: {
      return std::string();
    }
    case PROD_MAP_SCALAR_ESCAPED_NEXT: {
      std::string& result =
        std::any_cast<std::string&>(rhs.at(0));
      result.push_back(',');
      std::string& tail =
        std::any_cast<std::string&>(rhs.at(2));
      result += tail;
      return std::move(result);
    }
    case PROD_BSCALAR: {
      std::string& header =
        std::any_cast<std::string&>(rhs.at(0));
      std::string& rest =
        std::any_cast<std::string&>(rhs.at(4));
      std::string result(std::move(header));
      result.push_back('\n');
      result += rest;
      return result;
    }
    case PROD_BSCALAR_FIRST: {
      return std::move(rhs.at(0));
    }
    case PROD_BSCALAR_NEXT: {
      std::string& result =
        std::any_cast<std::string&>(rhs.at(0));
      std::string& item =
        std::any_cast<std::string&>(rhs.at(1));
      result += item;
      return std::move(result);
    }
    case PROD_BSCALAR_LINE: {
      std::string& any_star =
        std::any_cast<std::string&>(rhs.at(0));
      std::string result(std::move(any_star));
      result.push_back('\n');
      return result;
    }
    case PROD_BSCALAR_INDENT: {
      std::string const& indentation =
        std::any_cast<std::string const&>(rhs.at(0));
      std::string& content =
        std::any_cast<std::string&>(rhs.at(1));
      content.insert(0, indentation);
      for (std::size_t i = 0; i < content.size(); ++i) {
        if (content[i] == '\n') {
          content.insert(i + 1, indentation);
        }
      }
      return std::move(content);
    }
    case PROD_BSCALAR_HEADER_LITERAL:
    case PROD_BSCALAR_HEADER_FOLDED: {
      return std::move(rhs.at(1));
    }
    case PROD_BSCALAR_HEAD_EMPTY:
    case PROD_DQUOTED_EMPTY:
    case PROD_SQUOTED_EMPTY:
    case PROD_ANY_EMPTY:
    case PROD_DESCAPE_EMPTY:
    case PROD_SESCAPE_EMPTY:
    case PROD_SCALAR_TAIL_EMPTY: {
      return std::string();
    }
    case PROD_BSCALAR_HEAD_NEXT:
    case PROD_DQUOTED_NEXT:
    case PROD_SQUOTED_NEXT:
    case PROD_ANY_NEXT:
    case PROD_SCALAR_TAIL_NEXT: {
      std::string& result =
        std::any_cast<std::string&>(rhs.at(0));
      char c =
        std::any_cast<char>(rhs.at(1));
      result.push_back(c);
      return std::move(result);
    }
    case PROD_DESCAPE_NEXT:
    case PROD_SESCAPE_NEXT: {
      std::string& result =
        std::any_cast<std::string&>(rhs.at(0));
      std::string& next =
        std::any_cast<std::string&>(rhs.at(1));
      result += next;
      return std::move(result);
    }
    case PROD_BSCALAR_HEAD_OTHER: {
      return rhs.at(0);
    }
    case PROD_BSCALAR_HEAD_DASH: {
      return '-';
    }
    case PROD_DESCAPE: {
      char c =
        std::any_cast<char>(rhs.at(1));
      if (c == 't') c = '\t';
      if (c == 'n') c = '\n';
      std::string result;
      result.push_back(c);
      std::string& rest =
        std::any_cast<std::string&>(rhs.at(2));
      result += rest;
      return result;
    }
    case PROD_SESCAPE: {
      std::string result;
      result.push_back('\'');
      std::string& rest =
        std::any_cast<std::string&>(rhs.at(2));
      result += rest;
      return result;
    }
    case PROD_SCALAR_TAIL_SPACE:
    case PROD_SCALAR_TAIL_OTHER: {
      return rhs.at(0);
    }
    case PROD_SCALAR_TAIL_DOT: {
      return '.';
    }
    case PROD_SCALAR_TAIL_DASH: {
      return '-';
    }
    case PROD_SCALAR_TAIL_SQUOT:
    case PROD_DQUOTED_SQUOT:
    case PROD_ANY_SQUOT: {
      return '\'';
    }
    case PROD_DESCAPED_DQUOT:
    case PROD_SQUOTED_DQUOT:
    case PROD_ANY_DQUOT: {
      return '"';
    }
    case PROD_DESCAPED_SLASH:
    case PROD_SQUOTED_SLASH:
    case PROD_ANY_SLASH: {
      return '\\';
    }
    case PROD_DESCAPED_DQUOTED:
    case PROD_DQUOTED_COMMON:
    case PROD_SQUOTED_COMMON:
    case PROD_ANY_COMMON: {
      return rhs.at(0);
    }
    case PROD_COMMON_SPACE: {
      return rhs.at(0);
    }
    case PROD_COMMON_COLON: {
      return ':';
    }
    case PROD_COMMON_DOT: {
      return '.';
    }
    case PROD_COMMON_DASH: {
      return '-';
    }
    case PROD_COMMON_PIPE: {
      return '|';
    }
    case PROD_COMMON_LSQUARE: {
      return '[';
    }
    case PROD_COMMON_RSQUARE: {
      return ']';
    }
    case PROD_COMMON_LCURLY: {
      return '{';
    }
    case PROD_COMMON_RCURLY: {
      return '}';
    }
    case PROD_COMMON_RANGLE: {
      return '>';
    }
    case PROD_COMMON_COMMA: {
      return ',';
    }
    case PROD_COMMON_PERCENT: {
      return '%';
    }
    case PROD_COMMON_EXCL: {
      return '!';
    }
    case PROD_COMMON_OTHER: {
      return rhs.at(0);
    }
  }
  return std::any();
}

map reader::read_stream(
    std::istream& stream,
    std::string const& stream_name_in)
{
  return std::any_cast<map&&>(
      m_impl.read_stream(stream, stream_name_in));
}

map reader::read_string(
    std::string const& string,
    std::string const& string_name)
{
  return std::any_cast<map&&>(
      m_impl.read_string(string, string_name));
}

map reader::read_file(
      std::filesystem::path const& file_path)
{
  return std::any_cast<map&&>(
      m_impl.read_file(file_path));
}

}  // end namespace yaml
}  // end namespace parsegen
