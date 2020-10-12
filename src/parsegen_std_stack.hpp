#ifndef PARSEGEN_STD_STACK_HPP
#define PARSEGEN_STD_STACK_HPP

#include <stack>

namespace parsegen {

template <typename T>
int size(std::stack<T> const& s) {
  return static_cast<int>(s.size());
}

}  // namespace parsegen

#endif
