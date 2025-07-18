#pragma once
#include <string>
#include <iostream>

namespace poker {
    namespace platform {
        
        // Terminal color codes
        enum class Color {
            RESET = 0,
            RED = 1,
            GREEN = 2,
            YELLOW = 3,
            BLUE = 4,
            MAGENTA = 5,
            CYAN = 6,
            WHITE = 7
        };

        // Platform detection
        bool isWindows();
        bool isMac();
        bool isLinux();

        // Terminal capabilities
        bool supportsColors();
        bool supportsUnicode();

        // Text formatting
        std::string colorize(const std::string& text, Color color);
        std::string bold(const std::string& text);
        std::string resetFormat();

        // Card symbol utilities
        std::string getSuitSymbol(int suit, bool unicode_fallback = true);
        std::string getSuitName(int suit);
        std::string getSuitColor(int suit);

        // Cross-platform console operations
        void clearScreen();
        void pauseExecution();
        void setConsoleTitle(const std::string& title);

        // Path utilities
        std::string getPathSeparator();
        std::string joinPath(const std::string& path1, const std::string& path2);

        // Error handling
        void handlePlatformError(const std::string& operation, const std::string& error_msg);

        // Terminal size
        struct TerminalSize {
            int width;
            int height;
        };
        TerminalSize getTerminalSize();

        // UTF-8 utilities
        bool isValidUTF8(const std::string& str);
        std::string toUTF8(const std::wstring& wstr);
        std::wstring fromUTF8(const std::string& str);
    }
}
