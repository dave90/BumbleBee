#pragma once
// Force-included for every translation unit (see CMakeLists.txt).
//
// The codebase was originally compiled with toolchains (e.g. Apple/LLVM
// libc++) whose standard headers transitively pull in many other standard
// headers. libstdc++ (GCC) is stricter and does not, so building there fails
// with "X is not a member of std" / "incomplete type" errors for symbols the
// sources use without a direct include. Rather than scatter dozens of
// individual includes across src/ and vendored third_party/, we provide the
// commonly-relied-upon standard headers here in one place.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <limits>
#include <algorithm>
#include <memory>
#include <atomic>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <format>
