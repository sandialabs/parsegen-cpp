cmake_minimum_required(VERSION 3.12.0...3.18.0)

project(ParseGen VERSION 1.0.0 LANGUAGES CXX)

if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR CMake)
  set(INSTALL_BIN_DIR ".")
  set(INSTALL_LIB_DIR ".")
else()
  set(INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
  set(INSTALL_BIN_DIR "bin")
  set(INSTALL_LIB_DIR "lib")
endif()

add_subdirectory(src)

configure_file(ParseGenConfig.cmake.in
  "${PROJECT_BINARY_DIR}/ParseGenConfig.cmake" @ONLY)
configure_file(ParseGenConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/ParseGenConfigVersion.cmake" @ONLY)

# Install the ParseGenConfig.cmake and ParseGenConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}/ParseGenConfig.cmake"
  "${PROJECT_BINARY_DIR}/ParseGenConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

install(EXPORT ParseGenTargets
    DESTINATION "${INSTALL_CMAKE_DIR}")
