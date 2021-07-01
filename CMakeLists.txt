cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(parsegen VERSION 1.0.0 LANGUAGES CXX)

if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR CMake)
  set(INSTALL_BIN_DIR ".")
  set(INSTALL_LIB_DIR ".")
else()
  set(INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
  set(INSTALL_BIN_DIR "bin")
  set(INSTALL_LIB_DIR "lib")
endif()

option(parsegen_WORKAROUND_GCC_FILESYSTEM "link to stdc++fs" OFF)

add_subdirectory(src)

configure_file(parsegen-config.cmake.in
  "${PROJECT_BINARY_DIR}/parsegen-config.cmake" @ONLY)
configure_file(parsegen-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/parsegen-config-version.cmake" @ONLY)

# Install the ParseGenConfig.cmake and ParseGenConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/parsegen-config.cmake"
  "${PROJECT_BINARY_DIR}/parsegen-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

install(EXPORT parsegen-targets
    DESTINATION "${INSTALL_CMAKE_DIR}")
