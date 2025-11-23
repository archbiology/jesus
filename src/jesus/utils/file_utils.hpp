#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace utils
{
    inline std::string readFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + path);

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    inline std::string basenameWithoutExtension(const std::string &filename)
    {
        std::filesystem::path p(filename);
        return p.stem().string();
    }
}
