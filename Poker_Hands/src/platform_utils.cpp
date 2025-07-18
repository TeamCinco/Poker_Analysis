#include "platform_utils.h"
#include <sstream>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
    #include <locale>
    #include <codecvt>
    #define IS_WINDOWS true
    #define IS_MAC false
    #define IS_LINUX false
#elif __APPLE__
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #define IS_WINDOWS false
    #define IS_MAC true
    #define IS_LINUX false
#else
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #define IS_WINDOWS false
    #define IS_MAC false
    #define IS_LINUX true
#endif

namespace poker {
    namespace platform {
        
        bool isWindows() {
            return IS_WINDOWS;
        }

        bool isMac() {
            return IS_MAC;
        }

        bool isLinux() {
            return IS_LINUX;
        }

        bool supportsColors() {
            #ifdef _WIN32
                // Windows 10 and later support ANSI colors
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == INVALID_HANDLE_VALUE) return false;
                
                DWORD dwMode = 0;
                if (!GetConsoleMode(hOut, &dwMode)) return false;
                
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                return SetConsoleMode(hOut, dwMode);
            #else
                // Unix-like systems generally support colors
                const char* term = getenv("TERM");
                if (!term) return false;
                
                std::string termStr(term);
                return termStr != "dumb" && termStr.find("color") != std::string::npos;
            #endif
        }

        bool supportsUnicode() {
            #ifdef _WIN32
                // Check if Windows console supports UTF-8
                UINT codepage = GetConsoleOutputCP();
                return codepage == CP_UTF8 || codepage == 65001;
            #else
                // Check locale for UTF-8 support
                const char* locale = getenv("LC_ALL");
                if (!locale) locale = getenv("LC_CTYPE");
                if (!locale) locale = getenv("LANG");
                
                if (locale) {
                    std::string localeStr(locale);
                    return localeStr.find("UTF-8") != std::string::npos || 
                           localeStr.find("utf8") != std::string::npos;
                }
                return true; // Default to true on Unix systems
            #endif
        }

        std::string colorize(const std::string& text, Color color) {
            if (!supportsColors()) {
                return text;
            }

            std::string colorCode;
            switch (color) {
                case Color::RED:     colorCode = "\033[31m"; break;
                case Color::GREEN:   colorCode = "\033[32m"; break;
                case Color::YELLOW:  colorCode = "\033[33m"; break;
                case Color::BLUE:    colorCode = "\033[34m"; break;
                case Color::MAGENTA: colorCode = "\033[35m"; break;
                case Color::CYAN:    colorCode = "\033[36m"; break;
                case Color::WHITE:   colorCode = "\033[37m"; break;
                default:             colorCode = "\033[0m";  break;
            }

            return colorCode + text + "\033[0m";
        }

        std::string bold(const std::string& text) {
            if (!supportsColors()) {
                return text;
            }
            return "\033[1m" + text + "\033[0m";
        }

        std::string resetFormat() {
            return supportsColors() ? "\033[0m" : "";
        }

        std::string getSuitSymbol(int suit, bool unicode_fallback) {
            if (supportsUnicode() && unicode_fallback) {
                switch (suit) {
                    case 0: return "♠";  // Spades
                    case 1: return "♥";  // Hearts
                    case 2: return "♦";  // Diamonds
                    case 3: return "♣";  // Clubs
                    default: return "?";
                }
            } else {
                // ASCII fallback
                switch (suit) {
                    case 0: return "S";  // Spades
                    case 1: return "H";  // Hearts
                    case 2: return "D";  // Diamonds
                    case 3: return "C";  // Clubs
                    default: return "?";
                }
            }
        }

        std::string getSuitName(int suit) {
            switch (suit) {
                case 0: return "Spades";
                case 1: return "Hearts";
                case 2: return "Diamonds";
                case 3: return "Clubs";
                default: return "Unknown";
            }
        }

        std::string getSuitColor(int suit) {
            switch (suit) {
                case 0: return "Black";   // Spades
                case 1: return "Red";     // Hearts
                case 2: return "Red";     // Diamonds
                case 3: return "Black";   // Clubs
                default: return "Unknown";
            }
        }

        void clearScreen() {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        }

        void pauseExecution() {
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }

        void setConsoleTitle(const std::string& title) {
            #ifdef _WIN32
                SetConsoleTitleA(title.c_str());
            #else
                std::cout << "\033]0;" << title << "\007";
            #endif
        }

        std::string getPathSeparator() {
            #ifdef _WIN32
                return "\\";
            #else
                return "/";
            #endif
        }

        std::string joinPath(const std::string& path1, const std::string& path2) {
            if (path1.empty()) return path2;
            if (path2.empty()) return path1;
            
            std::string separator = getPathSeparator();
            std::string result = path1;
            
            if (result.back() != separator[0]) {
                result += separator;
            }
            
            return result + path2;
        }

        void handlePlatformError(const std::string& operation, const std::string& error_msg) {
            std::cerr << colorize("ERROR", Color::RED) << " in " << operation << ": " << error_msg << std::endl;
            
            #ifdef _WIN32
                DWORD error_code = GetLastError();
                if (error_code != 0) {
                    std::cerr << "Windows Error Code: " << error_code << std::endl;
                }
            #endif
        }

        TerminalSize getTerminalSize() {
            TerminalSize size = {80, 24}; // Default values
            
            #ifdef _WIN32
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
                    size.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                    size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                }
            #else
                struct winsize ws;
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
                    size.width = ws.ws_col;
                    size.height = ws.ws_row;
                }
            #endif
            
            return size;
        }

        bool isValidUTF8(const std::string& str) {
            const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.c_str());
            size_t len = str.length();
            
            for (size_t i = 0; i < len; ++i) {
                if (bytes[i] <= 0x7F) {
                    continue; // ASCII character
                } else if ((bytes[i] & 0xE0) == 0xC0) {
                    // 2-byte sequence
                    if (i + 1 >= len || (bytes[i + 1] & 0xC0) != 0x80) return false;
                    i += 1;
                } else if ((bytes[i] & 0xF0) == 0xE0) {
                    // 3-byte sequence
                    if (i + 2 >= len || (bytes[i + 1] & 0xC0) != 0x80 || (bytes[i + 2] & 0xC0) != 0x80) return false;
                    i += 2;
                } else if ((bytes[i] & 0xF8) == 0xF0) {
                    // 4-byte sequence
                    if (i + 3 >= len || (bytes[i + 1] & 0xC0) != 0x80 || (bytes[i + 2] & 0xC0) != 0x80 || (bytes[i + 3] & 0xC0) != 0x80) return false;
                    i += 3;
                } else {
                    return false;
                }
            }
            
            return true;
        }

        std::string toUTF8(const std::wstring& wstr) {
            #ifdef _WIN32
                if (wstr.empty()) return std::string();
                
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
                std::string result(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &result[0], size_needed, NULL, NULL);
                return result;
            #else
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.to_bytes(wstr);
            #endif
        }

        std::wstring fromUTF8(const std::string& str) {
            #ifdef _WIN32
                if (str.empty()) return std::wstring();
                
                int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
                std::wstring result(size_needed, 0);
                MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size_needed);
                return result;
            #else
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.from_bytes(str);
            #endif
        }
    }
}
