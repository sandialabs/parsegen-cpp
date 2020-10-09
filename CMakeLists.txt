cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(PARSEGEN VERSION 0.0.0 LANGUAGES CXX)

add_library(parsegen
  parsegen_chartab.cpp
  parsegen_build_parser.cpp
  parsegen_finite_automaton.cpp
  parsegen_grammar.cpp
  parsegen_language.cpp
  parsegen_math_lang.cpp
  parsegen_parser.cpp
  parsegen_parser_graph.cpp
  parsegen_reader.cpp
  parsegen_regex.cpp
  parsegen_xml.cpp
  parsegen_yaml.cpp
  )

add_executable(parsegen-calc
  parsegen_calc.cpp
  )

set_property(TARGET parsegen PROPERTY CXX_STANDARD "17")
set_property(TARGET parsegen PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET parsegen PROPERTY CXX_EXTENSIONS OFF)

set_property(TARGET parsegen-calc PROPERTY CXX_STANDARD "17")
set_property(TARGET parsegen-calc PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET parsegen-calc PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(parsegen-calc parsegen)
