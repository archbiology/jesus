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
