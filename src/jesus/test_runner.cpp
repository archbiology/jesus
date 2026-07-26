#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <optional>
#include "utils/file_utils.hpp"

std::string runJesusInterpreter(const std::string &inputFile, bool useRepl, bool keepAst)
{
    std::string command =
        std::string("jesus") + (keepAst ? " --keep-ast " : " ") + "'" + inputFile + "' 2>&1";

    if (useRepl)
        command = "jesus --quiet < '" + inputFile + "' 2>&1";

    std::string result;

    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
        return "[ERROR] Failed to run interpreter";

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    pclose(pipe);

    return result;
}

bool shouldUseRepl(const std::filesystem::path &path)
{
    if (path.string().contains("tests/repl/"))
        return true;

    return false;
}

bool shouldKeepAst(const std::string &path)
{
    if (path.contains("tests/optimizer/"))
        return true;

    return false;
}

std::optional<std::string> loadFile(const std::string &path)
{
    std::ifstream in(path);
    if (!in.is_open())
        return std::nullopt;

    std::ostringstream ss;
    ss << in.rdbuf();

    return ss.str();
}

void showDiff(const std::string &expected, const std::string &actual)
{
    std::istringstream eStream(expected);
    std::istringstream aStream(actual);
    std::string eLine, aLine;
    int line = 1;

    while (std::getline(eStream, eLine) && std::getline(aStream, aLine))
    {
        if (eLine != aLine)
        {
            std::cout << "    Line " << line << " differs 🔴️:\n";
            std::cout << "      Expected: '" << eLine << "'\n";
            std::cout << "      Actual:   '" << aLine << "'\n";
        }
        else
        {
            std::cout << "Line " << line << " ok ✅️\n";
        }
        ++line;
    }
}

int main()
{
    std::string testDir = "tests";
    bool allPassed = true;
    std::string replPath = std::filesystem::current_path();

    for (const auto &entry : std::filesystem::recursive_directory_iterator(testDir))
    {
        utils::changeWorkingDirectory(replPath);

        if (entry.path().extension() == ".jesus")
        {
            std::string fullInputFile = entry.path();
            std::string fullExpectedFile = fullInputFile + ".expected";

            std::string inputFile = fullInputFile;
            std::string expectedFile = fullExpectedFile;

            bool useRepl = shouldUseRepl(entry.path());
            if (!useRepl)
            {
                utils::changeWorkingDirectory(entry.path().parent_path());
                inputFile = entry.path().filename();
                expectedFile = inputFile + ".expected";
            }

            bool keepAst = shouldKeepAst(fullInputFile);
            std::string output = runJesusInterpreter(inputFile, useRepl, keepAst);
            auto expectedOpt = loadFile(expectedFile);

            if (!expectedOpt.has_value())
            {
                std::cout << "\n";
                std::cout << "[⚠] Missing expected output file:\n";
                std::cout << "    " << fullExpectedFile << "\n\n";

                std::cout << "    Create it with:\n";
                std::cout << "      jesus "
                          << (useRepl ? "--quiet < " : (keepAst ? "--keep-ast " : "")) << "'"
                          << fullInputFile << "' &> '" << fullExpectedFile << "'\n\n";

                allPassed = false;
                continue;
            }

            std::string expected = *expectedOpt;
            if (output == expected)
            {
                std::cout << "[✓] Passed: " << inputFile << "\n";
            }
            else
            {
                std::cout << "[✗] Failed: " << inputFile << "\n";
                showDiff(expected, output);
                allPassed = false;
            }
        }
    }

    if (!allPassed)
    {
        return 1;
    }

    return 0;
}
