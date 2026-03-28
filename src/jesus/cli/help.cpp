#include <iostream>

namespace HelpCLI
{
    void printGeneralHelp()
    {
        std::cout << "Jesus Programming Language CLI\n\n";

        std::cout << "Usage:\n";
        std::cout << "  jesus <file>.jesus\n";
        std::cout << "  jesus <command>\n";
        std::cout << "  jesus <book> <reference>\n\n";

        std::cout << "Commands:\n";
        std::cout << "  bible    Show how to read Bible verses\n\n";

        std::cout << "Help:\n";
        std::cout << "  --help | -h | /? | help\n\n";

        std::cout << "Examples:\n";
        std::cout << "  jesus genesis 1:1\n";
        std::cout << "  jesus john 3:16\n";
        std::cout << "  jesus psalm 23\n\n";

        std::cout << "Run 'jesus bible' for full Bible usage and supported formats.\n\n";
    }
}
