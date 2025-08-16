#pragma once

#include <string>
#include <cctype>
#include <vector>

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

    /**
     * @brief Extracts each UTF-8 character (1 or 2 bytes for Portuguese letters)
     *
     * @param source UTF-8 encoded string.
     * @return std::vector<std::string> Vector of UTF-8 characters.
     */
    inline std::vector<std::string> to_utf8(const std::string &source)
    {
        std::vector<std::string> chars;
        size_t len;

        for (size_t i = 0; i < source.size();)
        {
            unsigned char c = source[i];
            len = 1;

            // -----------------------------------------------------------------
            // 'c' is an unsigned char representing one byte from a UTF-8 encoded string.
            // In UTF-8, a single "logical character" may take 1 to 4 bytes.
            // ASCII characters (a-z, 0-9, etc.) are 1 byte, with the most significant bit = 0.
            // Portuguese accented letters like ç, é, ã are 2 bytes in UTF-8.
            //
            // 0xF0 in binary: 1111 0000
            // 'c & 0xF0' masks the top 4 bits of the byte.
            // UTF-8 2-byte sequences always start with a byte whose top 4 bits are 1100 (binary for 0xC).
            //
            // 11000000 = 0xC0
            // c & 0xF0 → isolate the top 4 bits of the first byte.
            // Compare with 0xC0 → check if this byte signals a 2-byte UTF-8 sequence.
            // If true, len = 2 → this “logical character” spans 2 bytes
            // -----------------------------------------------------------------
            if ((c & 0xF0) == 0xC0)
                len = 2; // 2-byte character (Portuguese letters)

            // No need to handle >2 bytes if we don't want emojis

            chars.push_back(source.substr(i, len));
            i += len;
        }
        return chars;
    }

    inline bool is_alpha_utf8(const std::string &c)
    {
        if (c.empty())
            return false;

        // ASCII letters
        if ((c[0] >= 'a' && c[0] <= 'z') || (c[0] >= 'A' && c[0] <= 'Z'))
            return true;

        // Portuguese letters in UTF-8 (2 bytes)
        static const std::string portuguese_letters[] = {
            "á", "à", "ã", "â", "é", "ê", "í", "ó", "ô", "õ", "ú", "ç",
            "Á", "À", "Ã", "Â", "É", "Ê", "Í", "Ó", "Ô", "Õ", "Ú", "Ç"};

        for (const auto &p : portuguese_letters)
            if (c == p)
                return true;

        return false;
    }

    inline bool is_digit_utf8(const std::string &c)
    {
        return (c.size() == 1 && c[0] >= '0' && c[0] <= '9');
    }

}
