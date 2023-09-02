#pragma once

#include <string>

const std::string red = "\033[0;31m";
const std::string green = "\033[0;32m";
const std::string reset = "\033[0m";

#define GTEST_COUT std::cout << green << "[   INFO   ] "
#define GTEST_ENDL reset << std::endl