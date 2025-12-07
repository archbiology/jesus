#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>

std::string runJesusInterpreter(const std::string &inputFile)
{
    std::string command = "./jesus --quiet < " + inputFile + " 2>&1";
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

std::string loadFile(const std::string &path)
{
    std::ifstream in(path);
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
            std::cout << "Line " << line << " differs 🔴️:\n";
            std::cout << "  Expected: '" << eLine << "'\n";
            std::cout << "  Actual:   '" << aLine << "'\n";
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

    for (const auto &entry : std::filesystem::recursive_directory_iterator(testDir))
    {
        if (entry.path().extension() == ".jesus")
        {
            std::string inputFile = entry.path();
            std::string expectedFile = inputFile + ".expected";

            std::string output = runJesusInterpreter(inputFile);
            std::string expected = loadFile(expectedFile);

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
