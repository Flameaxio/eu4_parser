#include "strings.hpp"
#include <string>
#include <algorithm>
#include <cctype>

namespace Utils
{
    std::string strip(std::string &str)
    {
        auto start = str.begin();
        while (start != str.end() && (std::isspace(*start) || *start == '	'))
        {
            start++;
        }

        auto end = str.end();
        do
        {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));

        return std::string(start, end + 1);
    }

    std::string make_string(const char *x)
    {
            return x;
    }
}