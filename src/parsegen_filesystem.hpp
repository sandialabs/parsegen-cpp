#pragma once

#if (!defined(__clang__)) && defined(__GNUC__) && (__GNUC__ < 8)
#include <experimental/filesystem>
/* Yes, defining our own stuff in namespace std
 * is bad practice.
 * However, GCC forced our hand by shipping
 * half-baked C++17 support in GCC 7.
 * In addition, sysadmins forced our hand by
 * not installing anything newer than GCC 7 on
 * important HCP machines.
 * Once that is fixed, this offense will be rectified.
 */
namespace std {
namespace filesystem {
using namespace std::experimental::filesystem;
}
}
#else
#include <filesystem>
#endif
