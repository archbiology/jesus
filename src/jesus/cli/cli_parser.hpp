#pragma once

#include <string>
#include <vector>

struct ParsedCLI
{
    bool quiet = false;
    std::string filename;          // ex: "serve" or serve.jesus
    std::vector<std::string> args; // everything after the command
};

class CLIParser
{
public:
    ParsedCLI parse(int argc, char **argv);
};
