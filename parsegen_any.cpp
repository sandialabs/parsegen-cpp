#include "parsegen_any.hpp"

namespace parsegen {

const char* bad_any_cast::what() const noexcept { return "bad any cast"; }

}  // namespace parsegen
