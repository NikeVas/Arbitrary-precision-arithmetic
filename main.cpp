#include "biginteger.hpp"

#include <iostream>
#include <sstream>

int main() {
  while (true) {
    BigInteger lhs, rhs;
    std::string operation;

    try {
      std::cout << ">>> ";
      std::string str;
      std::getline(std::cin, str);
      std::stringstream buf(str);

      buf >> lhs >> operation >> rhs;
    } catch (std::runtime_error& error) {
      std::cerr << error.what() << "\n";
      continue;
    }

    if (operation == "+") {
      std::cout << lhs + rhs;

    } else if (operation == "-") {
      std::cout << lhs - rhs;

    } else {
      std::cerr << "Unsupported operation type\n";
    }
    std::cout << "\n";
  }
}