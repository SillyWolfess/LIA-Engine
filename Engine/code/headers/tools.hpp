#ifndef LIA_TOOLS
#define LIA_TOOLS
#include <string>
#include <codecvt>
#include <ranges>

namespace LIA {
    std::wstring s2ws(const std::string& str);
    std::string ws2s(const std::wstring& wstr);
    std::string trim(std::string s);
    inline void ltrim(std::string &s);
    inline void rtrim(std::string &s);
    std::string sanitizePath(std::string s);
    std::string sanitizeFolderPath(std::string s);
    std::string sanitizeFileName(std::string name, std::string extension);
    std::string replaceAll(const std::string& str,const std::string& from, const std::string& to);
    bool equals(std::string &s1, std::string s2);
    bool equals(std::string &s1, const char* s2);
    static class CmdColor {
        public:
            static std::string white() { return "\033[0m";}
            static std::string red() { return "\033[1;31m";}
            static std::string yellow() { return "\033[1;33m";}
            static std::string lila() { return "\033[1;95m";}
            static std::string gray() { return "\033[1;90m";}
            static std::string cyan() { return "\033[1;96m";}
    };
}
#endif