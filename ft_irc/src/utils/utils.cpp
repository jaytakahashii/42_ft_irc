#include "utils/utils.hpp"

#include <iostream>

#include "utils/color.hpp"

void printError(const std::string& message) {
  std::cerr << RED "Error: " << message << RESET << std::endl;
}
