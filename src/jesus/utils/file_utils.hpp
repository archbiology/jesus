#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace utils
{
    /**
     * @brief Returns false on invalid UTF-8, reporting the 1-based line of the offending byte.
     */
    inline bool isValidUtf8(const std::string &content, size_t &errorLine)
    {
        const auto *bytes = reinterpret_cast<const unsigned char *>(content.data());
        const size_t size = content.size();
        size_t line = 1;

        size_t byteIndex = 0;
        while (byteIndex < size)
        {
            if (bytes[byteIndex] == '\n')
                ++line;

            const unsigned char firstByte = bytes[byteIndex];

            // ASCII: fastest path
            if (firstByte < 0x80)
            {
                ++byteIndex;
                continue;
            }

            size_t length;
            unsigned char b1Min = 0x80;
            unsigned char b1Max = 0xBF;

            // Rejects overlong forms (C0/C1), surrogates (ED A0..) and > U+10FFFF (F5+)
            if (firstByte >= 0xC2 && firstByte <= 0xDF)
                length = 2;

            else if (firstByte == 0xE0)
            {
                length = 3;
                b1Min = 0xA0;
            }
            else if (firstByte >= 0xE1 && firstByte <= 0xEC)
                length = 3;

            else if (firstByte == 0xED)
            {
                length = 3;
                b1Max = 0x9F;
            }
            else if (firstByte >= 0xEE && firstByte <= 0xEF)
                length = 3;

            else if (firstByte == 0xF0)
            {
                length = 4;
                b1Min = 0x90;
            }
            else if (firstByte >= 0xF1 && firstByte <= 0xF3)
                length = 4;

            else if (firstByte == 0xF4)
            {
                length = 4;
                b1Max = 0x8F;
            }
            else
            {
                errorLine = line;
                return false;
            }

            if (byteIndex + length > size || bytes[byteIndex + 1] < b1Min || bytes[byteIndex + 1] > b1Max)
            {
                errorLine = line;
                return false;
            }

            for (size_t offset = 2; offset < length; ++offset)
            {
                if (bytes[byteIndex + offset] < 0x80 || bytes[byteIndex + offset] > 0xBF)
                {
                    errorLine = line;
                    return false;
                }
            }

            byteIndex += length;
        }

        return true;
    }

    inline void validateUtf8(const std::string &content, const std::string &path)
    {
        size_t errorLine = 0;
        if (!isValidUtf8(content, errorLine))
        {
            throw std::runtime_error(
                "Encoding error: '" + path + "' is not a valid UTF-8 file" +
                (errorLine > 0 ? " (first invalid byte found on line " + std::to_string(errorLine) + ")" : "") +
                ".\n\n"
                "Jesus only supports UTF-8 source files.\n"
                "Other encodings (e.g. ISO 8859-1 / Windows-1252) are rejected to avoid encoding ambiguity,\n"
                "wrong characters and confusing editor/Git behavior.\n\n"
                "Tip: re-save the file as UTF-8 in your editor and try again.");
        }
    }

    inline size_t findCRLF(const std::string &content)
    {
        for (size_t i = 0; i + 1 < content.size(); ++i)
        {
            if (content[i] == '\r' && content[i + 1] == '\n')
            {
                size_t line = 1;
                for (size_t j = 0; j < i; ++j)
                    if (content[j] == '\n')
                        ++line;

                return line;
            }
        }
        return 0;
    }

    inline void validateNoCRLF(const std::string &content, const std::string &path)
    {
        size_t errorLine = findCRLF(content);
        if (errorLine > 0)
        {
            throw std::runtime_error(
                "Line ending error: '" + path + "' contains CRLF (\\r\\n) line endings" +
                " (first CRLF found on line " + std::to_string(errorLine) +
                ").\n\n"
                "Jesus only supports LF (\\n) line endings.\n"
                "CRLF line endings (\\r\\n) are rejected to keep diffs clean and avoid\n"
                "silent whitespace changes that clutter version control history.\n\n"
                "Tip: configure your editor to use 'LF (Unix)' line endings and re-save the file.\n"
                "In VS Code, click the CRLF/LF indicator in the bottom-right status bar to switch.");
        }
    }

    inline bool isFile(const std::string &path)
    {
        try
        {
            return std::filesystem::exists(path) &&
                   std::filesystem::is_regular_file(path);
        }
        catch (const std::filesystem::filesystem_error &)
        {
            return false;
        }
    }

    inline std::string readFile(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + path);

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        // A UTF-8 BOM is accepted and ignored
        if (content.size() >= 3 && static_cast<unsigned char>(content[0]) == 0xEF &&
            static_cast<unsigned char>(content[1]) == 0xBB && static_cast<unsigned char>(content[2]) == 0xBF)
        {
            content.erase(0, 3);
        }

        validateUtf8(content, path);
        validateNoCRLF(content, path);

        return content;
    }

    inline std::string basenameWithoutExtension(const std::string &filename)
    {
        std::filesystem::path p(filename);
        return p.stem().string();
    }

    inline bool changeWorkingDirectory(const std::filesystem::path &path)
    {
        try
        {
            std::filesystem::current_path(path);
            return true;
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "Error changing directory: " << e.what() << "\n";
            return false;
        }
    }
}
