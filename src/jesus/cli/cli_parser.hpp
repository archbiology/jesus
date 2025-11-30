#pragma once

#include <string>
#include <vector>
#include "../understanding/core/REGISTER_FOR_UML.hpp"

REGISTER_FOR_UML(
    CLIParser,
    .methodsList({"parse"}));

struct ParsedCLI
{
    bool quiet = false;
    bool export_language_uml = false;
    std::string filename;          // ex: "serve" or serve.jesus
    std::vector<std::string> args; // everything after the command
};

class CLIParser
{
public:
    ParsedCLI parse(int argc, char **argv);
};
