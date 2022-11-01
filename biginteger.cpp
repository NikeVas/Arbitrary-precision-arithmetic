#include "biginteger.hpp"

#include <algorithm>
#include <exception>
#include <utility>

BigInteger::BigInteger() : BigInteger(0) {
}

BigInteger::BigInteger(std::string_view str) {
  if (str[0] == '-' || str[0] == '+') {
    negative_ = str[0] == '-';
    str = str.substr(1, str.size() - 1);
    if (str.empty()) {
      throw std::runtime_error("String not convertible to BigInteger");
    }
  }

  size_t leading_zero = 0;
  for (; leading_zero < str.size(); ++leading_zero) {
    if (ToDigit(str[leading_zero]) != 0) {
      break;
    }
  }

  if (leading_zero == str.size()) {
    data_.push_back(0);
    return;
  }

  str = str.substr(leading_zero, str.size() - leading_zero);

  data_.reserve(str.size() / DIGIT_COUNT + str.size() % DIGIT_COUNT == 0 ? 0 : 1);

  for (size_t i = 1; i <= str.size() / DIGIT_COUNT; ++i) {
    data_.push_back(DigitFromString(str.substr(str.size() - i * DIGIT_COUNT, DIGIT_COUNT)));
  }

  auto other_size = str.size() % DIGIT_COUNT;
  if (other_size != 0) {
    data_.push_back(DigitFromString(str.substr(0, other_size)));
  }
}

BigInteger BigInteger::operator-() const {
  BigInteger bigint = (*this);
  bigint.negative_ = !bigint.negative_;
  return bigint;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) & {
  if (negative_ == other.negative_) {
    AddBase(other);
  } else {
    negative_ = !negative_;
    operator-=(other);
    negative_ = !negative_;
  }

  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) & {
  if (negative_ == other.negative_) {
    DecBase(other);
  } else {
    negative_ = !negative_;
    operator+=(other);
    negative_ = !negative_;
  }

  return *this;
}

std::string BigInteger::ToString() const {
  if (IsZero()) {
    return "0";
  }

  std::string result;
  result.reserve(data_.size() * DIGIT_COUNT + 1);

  result += negative_ ? "-" : "";

  for (auto current = data_.rbegin(); current != data_.rend(); ++current) {
    auto str_dig = std::to_string(*current);
    if (current != data_.rbegin()) {
      result += std::string("0", DIGIT_COUNT - str_dig.length());
    }
    result += str_dig;
  }

  return result;
}

std::strong_ordering BigInteger::operator<=>(const BigInteger& other) const {
  if (IsZero() && other.IsZero()) {
    return std::strong_ordering::equal;
  }
  
  if (negative_ != other.negative_) {
    return negative_ ? std::strong_ordering::less : std::strong_ordering::greater;
  }

  if (data_.size() != other.data_.size()) {
    return (data_.size() < other.data_.size()) ^ (negative_) ? std::strong_ordering::less
                                                             : std::strong_ordering::greater;
  }

  for (auto i = data_.rbegin(), j = other.data_.rbegin(); i != data_.rend() && j != other.data_.rend(); ++i, ++j) {
    if (*i != *j) {
      return (*i < *j) ^ (negative_) ? std::strong_ordering::less : std::strong_ordering::greater;
    }
  }

  return std::strong_ordering::equal;
}

bool BigInteger::operator==(const BigInteger& other) const {
  return (*this <=> other) == std::strong_ordering::equal;
}

// Additional methods //-------------------------------------------------------

BigInteger::SumResult BigInteger::SumDigit(DigitType lhs, DigitType rhs, DigitType from_prev) {
  CalculateType res = lhs;
  res += rhs;
  res += from_prev;
  if (res >= BASE_SIZE) {
    return {static_cast<DigitType>(res - BASE_SIZE), 1};
  } else {
    return {static_cast<DigitType>(res), 0};
  }
}

BigInteger::DesResult BigInteger::DecDigit(DigitType lhs, DigitType rhs, DigitType to_next) {
  auto big_rhs = static_cast<CalculateType>(rhs) + to_next;
  CalculateType big_lhs = lhs;
  if (big_rhs > big_lhs) {
    auto res = BASE_SIZE + big_lhs - big_rhs;
    return {static_cast<DigitType>(res), 1};
  } else {
    return {static_cast<DigitType>(big_lhs - big_rhs), 0};
  }
}

void BigInteger::AddBase(const BigInteger& other) {
  // pre cond: negative == other.negative
  const auto& lhs = data_;
  const auto& rhs = other.data_;

  auto max_size = std::max(lhs.size(), rhs.size());
  auto min_size = std::min(lhs.size(), rhs.size());

  data_.resize(max_size, static_cast<DigitType>(0));

  DigitType to_next = 0;
  for (size_t i = 0; i < min_size; ++i) {
    auto sum = SumDigit(lhs[i], rhs[i], to_next);
    to_next = sum.to_next;
    data_[i] = sum.to_current;
  }

  const auto& biggest = lhs.size() > rhs.size() ? lhs : rhs;

  for (auto i = min_size; i < max_size; ++i) {
    auto sum = SumDigit(biggest[i], static_cast<DigitType>(0), to_next);
    to_next = sum.to_next;
    data_[i] = sum.to_current;
  }

  if (to_next != 0) {
    data_.push_back(to_next);
  }
}

void BigInteger::DecBase(const BigInteger& other) {
  // pre cond: negative == other.negative
  // |lhs| >= |rhs|
  bool is_swap = ((*this) < other) ^ negative_;
  if (is_swap) {
    negative_ = !negative_;
  }
  auto& lhs = is_swap ? other.data_ : data_;
  auto& rhs = is_swap ? data_ : other.data_;

  data_.resize(std::max(lhs.size(), rhs.size()), static_cast<DigitType>(0));

  DigitType from_next = 0;
  for (size_t i = 0; i < rhs.size(); ++i) {
    auto dec = DecDigit(lhs[i], rhs[i], from_next);
    from_next = dec.from_next;
    data_[i] = dec.to_current;
  }

  for (size_t i = rhs.size(); i < lhs.size(); ++i) {
    auto dec = DecDigit(lhs[i], static_cast<DigitType>(0), from_next);
    from_next = dec.from_next;
    data_[i] = dec.to_current;
  }

  // from_next must be zero

  ReduceLeadingZero();
}

void BigInteger::ReduceLeadingZero() {
  for (auto i = data_.rbegin(); i != data_.rend(); ++i) {
    if (*i != 0) {
      data_.resize(data_.rend() - i);
      return;
    }
  }
  data_.resize(1);
  data_[0] = 0;
}

char BigInteger::ToDigit(char c) const {
  if ('0' <= c && c <= '9') {
    return c - '0';
  } else {
    throw std::runtime_error("String not convertible to BigInteger");
  }
}

BigInteger::DigitType BigInteger::DigitFromString(std::string_view str) {
  DigitType res = 0;
  for (auto c : str) {
    res *= 10;
    res += ToDigit(c);
  }
  return res;
}

bool BigInteger::IsZero() const {
  return (data_.size() == 1 && data_[0] == 0);
}

// Out functions //------------------------------------------------------------

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
  BigInteger bigint = lhs;
  bigint += rhs;
  return bigint;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
  BigInteger bigint = lhs;
  bigint -= rhs;
  return bigint;
}

std::istream& operator>>(std::istream& stream, BigInteger& bigint) {
  std::string str;
  stream >> str;
  bigint = BigInteger(str);

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const BigInteger& bigint) {
  stream << bigint.ToString();

  return stream;
}

BigInteger operator""_bi(const char* str, std::size_t size) {
  return BigInteger({str, size});
}

BigInteger operator""_bi(unsigned long long int value) {  // NOLINT
  return BigInteger(value);
}