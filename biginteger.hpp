#pragma once

#include "helper.hpp"

#include <concepts>
#include <iostream>
#include <string>
#include <compare>
#include <cstdint>
#include <vector>
#include <limits>

class BigInteger {
 public:
  BigInteger();

  BigInteger(const BigInteger&) = default;
  BigInteger(BigInteger&&) = default;
  BigInteger& operator=(const BigInteger&) & = default;
  BigInteger& operator=(BigInteger&&) & = default;
  ~BigInteger() = default;

  explicit BigInteger(std::string_view);

  template <std::integral T>
  BigInteger(T);  // NOLINT

  BigInteger operator-() const;

  // BigInteger& operator++() &;
  // BigInteger operator++(int) &;
  // BigInteger& operator--() &;
  // BigInteger operator--(int) &;

  BigInteger& operator+=(const BigInteger&) &;
  BigInteger& operator-=(const BigInteger&) &;
  // BigInteger& operator*=(const BigInteger&) &;
  // BigInteger& operator/=(const BigInteger&) &;
  // BigInteger& operator%=(const BigInteger&) &;

  friend std::istream& operator>>(std::istream&, BigInteger&);
  friend std::ostream& operator<<(std::ostream&, const BigInteger&);

  std::string ToString() const;

  std::strong_ordering operator<=>(const BigInteger&) const;
  bool operator==(const BigInteger&) const;

 private:
  using CalculateType = uint64_t;
  using DigitType = uint32_t;

  static constexpr size_t DIGIT_COUNT = 9;
  // BASE_SIZE = 10 ^ DIGIT_COUNT
  static constexpr DigitType BASE_SIZE = detail::CalculateBase<DigitType, CalculateType>(DIGIT_COUNT);
  static constexpr std::make_signed_t<DigitType> SIGNED_BASE_SIZE = BASE_SIZE;

  static_assert(BASE_SIZE * BASE_SIZE <= std::numeric_limits<CalculateType>::max());

 private:  // Addition functions
  struct SumResult {
    DigitType to_current;
    DigitType to_next;
  };

  static SumResult SumDigit(DigitType lhs, DigitType rhs, DigitType from_prev);

  struct DesResult {
    DigitType to_current;
    DigitType from_next;
  };

  static DesResult DecDigit(DigitType lhs, DigitType rhs, DigitType to_next);

  void AddBase(const BigInteger&);
  void DecBase(const BigInteger&);

  void ReduceLeadingZero();

  char ToDigit(char c) const;
  DigitType DigitFromString(std::string_view);

  bool IsZero() const;

 private:  // Private fields
  std::vector<DigitType> data_;
  bool negative_ = false;
};

BigInteger operator+(const BigInteger&, const BigInteger&);
BigInteger operator-(const BigInteger&, const BigInteger&);
// BigInteger operator*(const BigInteger&, const BigInteger&);
// BigInteger operator/(const BigInteger&, const BigInteger&);
// BigInteger operator%(const BigInteger&, const BigInteger&);

std::istream& operator>>(std::istream&, BigInteger&);
std::ostream& operator<<(std::ostream&, const BigInteger&);

BigInteger operator""_bi(const char* str, std::size_t size);
BigInteger operator""_bi(unsigned long long int value);  // NOLINT

// Template implementation //--------------------------------------------------

template <std::integral T>
BigInteger::BigInteger(T value) {
  if (value == 0) {
    data_.push_back(0);
    return;
  }
  if (value < 0) {
    negative_ = true;
  }
  while (value != 0) {
    data_.push_back(value % SIGNED_BASE_SIZE * (negative_ ? -1 : 1));
    value /= SIGNED_BASE_SIZE;
  }
}
