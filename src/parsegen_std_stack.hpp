#pragma once

#include <stack>

namespace parsegen {

template <typename T>
int isize(std::stack<T> const& s) {
  return static_cast<int>(s.size());
}

}  // namespace parsegen
