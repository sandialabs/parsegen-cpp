cmake_minimum_required(VERSION 3.15.0...3.21.2)

project(parsegen VERSION 2.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(parsegen_WORKAROUND_GCC_FILESYSTEM "link to stdc++fs" OFF)

add_subdirectory(src)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/parsegen-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parsegen
)
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/parsegen-config-version.cmake"
  COMPATIBILITY AnyNewerVersion)

install(FILES
  "${PROJECT_BINARY_DIR}/parsegen-config.cmake"
  "${PROJECT_BINARY_DIR}/parsegen-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parsegen)

install(
  EXPORT parsegen-targets
  NAMESPACE parsegen::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parsegen)
