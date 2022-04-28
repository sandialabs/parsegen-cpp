set(PARSEGEN_HEADERS
  parsegen_language.hpp
  parsegen_filesystem.hpp
  parsegen_parser.hpp
  parsegen_finite_automaton.hpp
  parsegen_table.hpp
  parsegen_std_vector.hpp
  parsegen_grammar.hpp
  parsegen_parser_tables.hpp
  parsegen_shift_reduce_tables.hpp
  parsegen_regex.hpp
  parsegen_xml.hpp
  parsegen_yaml.hpp
  parsegen_math_lang.hpp
  parsegen_error.hpp
  parsegen_object_pointer.hpp
  parsegen_string.hpp
  parsegen.hpp
  )

add_library(parsegen
  parsegen_chartab.cpp
  parsegen_string.cpp
  parsegen_build_parser.cpp
  parsegen_finite_automaton.cpp
  parsegen_grammar.cpp
  parsegen_language.cpp
  parsegen_math_lang.cpp
  parsegen_shift_reduce_tables.cpp
  parsegen_parser_graph.cpp
  parsegen_parser.cpp
  parsegen_regex.cpp
  parsegen_xml.cpp
  parsegen_yaml.cpp
  parsegen_error.cpp
  )

target_compile_features(parsegen PUBLIC cxx_std_17)
set_target_properties(parsegen PROPERTIES
  PUBLIC_HEADER "${PARSEGEN_HEADERS}")
target_include_directories(parsegen
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

try_compile(
  compiler_supports_filesystem
  "${CMAKE_CURRENT_BINARY_DIR}"
  SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/compiler_supports_filesystem.cpp"
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED TRUE
  CXX_EXTENSIONS FALSE)

if (NOT compiler_supports_filesystem)
  target_link_libraries(parsegen PUBLIC stdc++fs)
endif()

add_executable(parsegen-calc
  parsegen_calc.cpp
  )

target_compile_features(parsegen-calc PUBLIC cxx_std_17)

target_link_libraries(parsegen-calc PRIVATE parsegen)

install(
  TARGETS parsegen parsegen-calc
  EXPORT parsegen-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
