cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(PARSEGEN VERSION 0.0.0 LANGUAGES CXX)

add_library(parsegen
  Omega_h_any.cpp
  Omega_h_chartab.cpp
  Omega_h_build_parser.cpp
  Omega_h_finite_automaton.cpp
  Omega_h_grammar.cpp
  Omega_h_language.cpp
  Omega_h_math_lang.cpp
  Omega_h_parser.cpp
  Omega_h_parser_graph.cpp
  Omega_h_reader.cpp
  Omega_h_regex.cpp
  Omega_h_xml.cpp
  Omega_h_yaml.cpp
  )

set_property(TARGET parsegen PROPERTY CXX_STANDARD "17")
set_property(TARGET parsegen PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET parsegen PROPERTY CXX_EXTENSIONS OFF)
