#ifndef PARSEGEN_TABLE_HPP
#define PARSEGEN_TABLE_HPP

#include "parsegen_std_vector.hpp"

namespace parsegen {

/* pretty simple 2D array */
template <typename T>
struct table {
  std::vector<T> data;
  int ncols;
  using reference = typename std::vector<T>::reference;
  using const_reference = typename std::vector<T>::const_reference;
  table() = default;
  table(int ncols_init, int nrows_reserve) : ncols(ncols_init) {
    assert(0 <= ncols_init);
    reserve(data, ncols * nrows_reserve);
  }
};

template <typename T>
int get_nrows(table<T> const& t) {
  assert(t.ncols > 0);
  assert(size(t.data) % t.ncols == 0);
  return isize(t.data) / t.ncols;
}

template <typename T>
int get_ncols(table<T> const& t) {
  return t.ncols;
}

template <typename T>
void resize(table<T>& t, int new_nrows, int new_ncols) {
  assert(new_ncols == t.ncols);  // pretty specialized right now
  parsegen::resize(t.data, new_nrows * t.ncols);
}

template <typename T>
typename table<T>::reference at(table<T>& t, int row, int col) {
  assert(0 <= col);
  assert(col < t.ncols);
  assert(0 <= row);
  assert(row < get_nrows(t));
  return parsegen::at(t.data, row * t.ncols + col);
}

template <typename T>
typename table<T>::const_reference at(table<T> const& t, int row, int col) {
  assert(0 <= col);
  assert(col < t.ncols);
  assert(0 <= row);
  assert(row < get_nrows(t));
  return parsegen::at(t.data, row * t.ncols + col);
}

}  // namespace parsegen

#endif
