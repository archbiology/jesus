#include "string_utils.hpp"

namespace utils
{
    void replaceAll(std::string &text, const std::string &search, const std::string &replacement)
    {
        if (search.empty())
            return;

        size_t start = 0;
        while ((start = text.find(search, start)) != std::string::npos)
        {
            text.replace(start, search.length(), replacement);
            start += replacement.length();
        }
    }
}
