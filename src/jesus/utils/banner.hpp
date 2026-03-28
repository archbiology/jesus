#include <iostream>
#include "terminal/terminal.hpp"
#include "terminal/color.hpp"

namespace Banner
{
    inline void show(const std::string &version, const std::string &commit, bool quiet)
    {
        if (quiet)
            return;

        std::cout << "Jesus Programming Language " << version << " (י ש ו ע ה) [" << commit << "]\n\n";

        const bool useColor = terminal::supportsColor();
        if (useColor)
        {
            std::cout
                << "\"If you confess with your mouth '"
                << terminal::color::bold
                << "Jesus Christ is my Lord"
                << terminal::color::bold_reset
                << "', \n";
        }
        else
        {
            std::cout << "\"If you confess with your mouth 'Jesus Christ is my Lord', \n";
        }

        std::cout << "and believe in your heart that God raised him from the dead,\n";
        std::cout << "you will be saved.\" — Romans 10:9\n";
        // std::cout << "\"May the peace of the Lord Jesus Christ, the Prince of Peace, rest upon you.\"\n";
        std::cout << std::endl;
    }
}
