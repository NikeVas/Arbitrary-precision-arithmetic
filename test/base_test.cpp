#include "biginteger.hpp"

#include <cassert>

void TestZero() {
  BigInteger zero(0);
  assert(zero == 0);
  assert(0 == zero);
  assert(zero.ToString() == "0");

  BigInteger negative_zero = -zero;
  assert(negative_zero == 0);
  assert(0 == negative_zero);
  assert(negative_zero.ToString() == "0");
}

void CheckString(std::string_view str, std::string_view result) {
  BigInteger test(str);
  assert(test.ToString() == result);
}

void TestStringConversion() {
  CheckString("0", "0");
  CheckString("-0", "0");
  CheckString("123456789123456789", "123456789123456789");
  CheckString("-123456789123456789", "-123456789123456789");
  CheckString("000000000000000000", "0");
  CheckString("-000000000000000000", "0");
  CheckString("000000000000000000123456789", "123456789");
  CheckString("-000000000000000000123456789", "-123456789");
  CheckString("+000000000000000000123456789", "123456789");
}

void CheckException(std::string_view str) {
  try {
    BigInteger test(str);
    assert(false);
  } catch (std::runtime_error&) {

  }
}

void TestWrongStringConversion() {
  CheckException("Test");
  CheckException("++0");
  CheckException("--0");
  CheckException("-");
  CheckException("+");
}

void CheckConversion(auto value) {
  BigInteger test(value);
  assert(test.ToString() == std::to_string(value));
}

void TestIntegralConversion() {
  CheckConversion(0);
  CheckConversion(-0);
  CheckConversion(123456789);
  CheckConversion(123456789U);
  CheckConversion(123456789UL);
  CheckConversion(123456789ULL);
  CheckConversion(std::numeric_limits<int8_t>::max());
  CheckConversion(std::numeric_limits<int8_t>::min());
  CheckConversion(std::numeric_limits<uint8_t>::max());
  CheckConversion(std::numeric_limits<uint8_t>::min());
  CheckConversion(std::numeric_limits<size_t>::max());
  CheckConversion(std::numeric_limits<size_t>::min());
  CheckConversion(std::numeric_limits<int64_t>::max());
  CheckConversion(std::numeric_limits<int64_t>::min());
}

void TestComparaison() {
  BigInteger a(123456789);
  BigInteger b("123456789");
  assert(a == b);
  assert(a == 123456789);
  assert(b == 123456789);
  assert(a == "123456789"_bi);
  assert(b == "123456789"_bi);

  assert(b == a);
  assert(123456789 == a);
  assert(123456789 == b);
  assert("123456789"_bi == a);
  assert("123456789"_bi == b);

  {
    BigInteger big_int("123456789123456789123456789");
    BigInteger other_big_int("123456789123456789123456789");
    assert(big_int == other_big_int);
    assert(!(big_int != other_big_int));
    assert(big_int <= other_big_int);
    assert(big_int >= other_big_int);
    assert(!(big_int < other_big_int));
    assert(!(big_int > other_big_int));
  }

  {
    BigInteger big_int("123456789123456789123456789");
    BigInteger other_big_int("-123456789123456789123456789");
    assert(!(big_int == other_big_int));
    assert(big_int != other_big_int);
    assert(!(big_int <= other_big_int));
    assert(big_int >= other_big_int);
    assert(!(big_int < other_big_int));
    assert(big_int > other_big_int);
  }

  {
    BigInteger big_int("-123456789123456789123456789");
    BigInteger other_big_int("-123456789123456789123456789");
    assert(big_int == other_big_int);
    assert(!(big_int != other_big_int));
    assert(big_int <= other_big_int);
    assert(big_int >= other_big_int);
    assert(!(big_int < other_big_int));
    assert(!(big_int > other_big_int));
  }

  {
    BigInteger big_int("0");
    BigInteger other_big_int("123456789123456789123456789");
    assert(!(big_int == other_big_int));
    assert(big_int != other_big_int);
    assert(big_int <= other_big_int);
    assert(!(big_int >= other_big_int));
    assert(big_int < other_big_int);
    assert(!(big_int > other_big_int));
  }

  {
    BigInteger big_int("0");
    BigInteger other_big_int("-123456789123456789123456789");
    assert(!(big_int == other_big_int));
    assert(big_int != other_big_int);
    assert(!(big_int <= other_big_int));
    assert(big_int >= other_big_int);
    assert(!(big_int < other_big_int));
    assert(big_int > other_big_int);
  }
}

void TestSum() {
  BigInteger bigint("123456789123456789");

  assert(0_bi + 0_bi == 0_bi);
  assert(0_bi + bigint == bigint);
  assert(bigint + 0_bi == bigint);

  assert(0 + bigint == bigint);
  assert(bigint + 0 == bigint);

  bigint += 0_bi;
  assert(bigint == "123456789123456789"_bi);

  assert(bigint + bigint == "246913578246913578"_bi);

  assert("111111111111111111111111111"_bi + 
         "999999999999999999999999999"_bi ==
         "1111111111111111111111111110"_bi);

  assert("-111111111111111111111111111"_bi + 
         "-999999999999999999999999999"_bi ==
         "-1111111111111111111111111110"_bi);
  
  assert("+999999999999999999999999999"_bi + 
         "-111111111111111111111111111"_bi ==
         "+888888888888888888888888888"_bi);

  assert("-111111111111111111111111111"_bi + 
         "+999999999999999999999999999"_bi ==
         "+888888888888888888888888888"_bi);
  
  assert("+1000000000000000000000000001"_bi + 
         "-1000000000000000000000000002"_bi ==
         "-1"_bi);

  assert("-1000000000000000000000000001"_bi + 
         "+1000000000000000000000000002"_bi ==
         "+1"_bi);

  assert("1"_bi + 
         "1000000000000000000000000002"_bi ==
         "1000000000000000000000000003"_bi);

  assert("1245434567898890098765434567876556543234567890098765434567654"_bi + 
         "2543436757837973249583265789573425489678934767369340274309877"_bi ==
         "3788871325736863348348700357449982032913502657468105708877531"_bi);

  assert("+1245434567898789098678876543212345689098765"_bi + 
         "-987898768987898765434567898765678987659876566543245"_bi ==
         "-987898767742464197535778800086802444447530877444480"_bi);
}

void TestDec() {
  BigInteger bigint("123456789123456789");

  assert(0_bi - 0_bi == 0_bi);
  assert(0_bi - bigint == -bigint);
  assert(bigint - 0_bi == bigint);

  assert(0 - bigint == -bigint);
  assert(bigint - 0 == bigint);

  bigint -= 0_bi;
  assert(bigint == "123456789123456789"_bi);
  
  assert(bigint - bigint == "0"_bi);

  assert("+111111111111111111111111111"_bi -
         "+999999999999999999999999999"_bi ==
         "-888888888888888888888888888"_bi);

  assert("-111111111111111111111111111"_bi - 
         "-999999999999999999999999999"_bi ==
         "+888888888888888888888888888"_bi);

  assert("+111111111111111111111111111"_bi - 
         "-999999999999999999999999999"_bi ==
         "+1111111111111111111111111110"_bi);

  assert("+1000000000000000000000000001"_bi -
         "+1000000000000000000000000002"_bi ==
         "-1"_bi);

  assert("-1000000000000000000000000001"_bi - 
         "+1000000000000000000000000002"_bi ==
         "-2000000000000000000000000003"_bi);

  assert("2345678900987654345678765456789876543212345678876543456"_bi - 
         "987898987878765434567654324567654345676543456765456765445676543456786543456765"_bi ==
         "-987898987878765434567651978888753358022197777999999975569133331111107666913309"_bi);

  assert("-98765456789876545678987654323456789876543234567765432347876543"_bi - 
         "-9876556776545654543454345434565"_bi ==
         "-98765456789876545678987654323446913319766688913221978002441978"_bi);
}

int main() {
  TestZero();
  TestStringConversion();
  TestWrongStringConversion();
  TestIntegralConversion();
  TestComparaison();
  TestSum();
  TestDec();
}