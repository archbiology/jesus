#pragma once

#include <string>
#include <cctype>

namespace utils
{
    /**
     * @brief Removes leading and trailing whitespace from the input string.
     *
     * @param str The string to trim.
     * @return std::string A new string with leading and trailing whitespace removed.
     */
    inline std::string trim(const std::string &str)
    {
        size_t start = 0;
        while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
        {
            ++start;
        }

        if (start == str.size())
            return "";

        size_t end = str.size() - 1;
        while (end > start && std::isspace(static_cast<unsigned char>(str[end])))
        {
            --end;
        }

        return str.substr(start, end - start + 1);
    }
}
