#ifndef COMMON_8f348b7bd74e427f8bfbaa7080b12ea3
#define COMMON_8f348b7bd74e427f8bfbaa7080b12ea3

#include "Construction.h"
#include "Macros.h"
#include "Swap.h"

template <typename T, std::size_t N> constexpr std::size_t countof (T const (&)[N]) noexcept { return N; }

#endif // Include guard