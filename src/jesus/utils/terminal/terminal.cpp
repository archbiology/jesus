#include "terminal.hpp"

#if defined(_WIN32)
    #include <io.h>
    #include <windows.h>
#else
    #include <unistd.h>
    #include <cstdio>
#endif

namespace terminal
{
    bool supportsColor()
    {
#if defined(_WIN32)
        // Check if stderr is a console
        if (!_isatty(_fileno(stderr)))
            return false;

        // Enable ANSI escape sequences on Windows 10+
        HANDLE hOut = GetStdHandle(STD_ERROR_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE)
            return false;

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode))
            return false;

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        return SetConsoleMode(hOut, dwMode);
#else
        return isatty(fileno(stderr));
#endif
    }
}
