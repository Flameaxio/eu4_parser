#include "numbers.hpp"
#include <algorithm>
#include <cctype>   // For isdigit
#include <cstdlib>  // For strtol

namespace Utils {
    bool is_number(const std::string& s)
    {
        return !s.empty() && std::find_if(s.begin(), 
            s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
    }
}