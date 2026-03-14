#pragma once

#include <string>
#include <vector>
#include "../understanding/core/REGISTER_FOR_UML.hpp"
#include "../understanding/scripture/language.hpp"

REGISTER_FOR_UML(
    CLIParser,
    .methodsList({"parse"}));

struct ParsedCLI
{
    bool quiet = false;
    bool export_language_uml = false;
    std::string filename;          // ex: "serve" or serve.jesus
    std::vector<std::string> args; // everything after the command

    bool isScripture = false;
    std::string scriptureText;                                   // raw text: "john 3:16-18 4:1"
    HolyBible::Language language = HolyBible::Language::English; // default
};

class CLIParser
{
public:
    ParsedCLI parse(int argc, char **argv);

    bool looksLikeBibleBook(const std::string &token);
};
