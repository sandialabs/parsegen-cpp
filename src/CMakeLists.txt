set(PARSEGEN_HEADERS
  parsegen_language.hpp
  parsegen_reader.hpp
  parsegen_finite_automaton.hpp
  parsegen_table.hpp
  parsegen_std_vector.hpp
  parsegen_grammar.hpp
  parsegen_reader_tables.hpp
  parsegen_parser.hpp
  parsegen_regex.hpp
  parsegen_xml.hpp
  parsegen_yaml.hpp
  parsegen_math_lang.hpp
  )

add_library(parsegen
  parsegen_chartab.cpp
  parsegen_string.cpp
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

target_compile_features(parsegen PUBLIC cxx_std_17)
set_target_properties(parsegen PROPERTIES
  PUBLIC_HEADER "${PARSEGEN_HEADERS}")
target_include_directories(parsegen
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if (parsegen_WORKAROUND_GCC_FILESYSTEM)
  target_link_libraries(parsegen PUBLIC stdc++fs)
endif()

add_executable(parsegen-calc
  parsegen_calc.cpp
  )

target_compile_features(parsegen-calc PUBLIC cxx_std_17)

target_link_libraries(parsegen-calc PRIVATE parsegen)

install(
  TARGETS parsegen
  EXPORT parsegen-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
