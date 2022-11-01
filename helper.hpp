#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>

namespace detail {

template <typename DigitType, typename CalculateType>
constexpr DigitType CalculateBase(size_t digit_count) {
  CalculateType res = 1;
  for (size_t i = 0; i < digit_count; ++i) {
    res *= 10;
    if (res > std::numeric_limits<DigitType>::max()) {
      throw std::logic_error("To much digit count");
    }
  }

  return static_cast<DigitType>(res);
}
}  // namespace detail