#include "MessageRedirection.h"

#include "Zenova/Platform.h"

#include <iostream>
#include <Windows.h>

namespace Zenova {
    MessageRedirection::MessageRedirection() {
        if(!AllocConsole()) {
            Platform::ErrorPrinter();
            return;
        }

        if(!console) {
            //Handle std::cout, std::clog, std::cerr, std::cin
            freopen_s(&console, "CONOUT$", "w", stdout);
            freopen_s(&console, "CONOUT$", "w", stderr);
            freopen_s(&console, "CONIN$", "r", stdin);

            HANDLE hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            HANDLE hConIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
            SetStdHandle(STD_ERROR_HANDLE, hConOut);
            SetStdHandle(STD_INPUT_HANDLE, hConIn);

            std::cout.clear();
            std::clog.clear();
            std::cerr.clear();
            std::cin.clear();
            std::wcout.clear();
            std::wclog.clear();
            std::wcerr.clear();
            std::wcin.clear();
        }
    }

    MessageRedirection::~MessageRedirection() {
        if(!FreeConsole()) {
            Platform::ErrorPrinter();
        }

        if(console) {
            fclose(console);
        }
    }
}