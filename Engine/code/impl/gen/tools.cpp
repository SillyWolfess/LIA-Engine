#include "tools.hpp"
#include <string>
#include <algorithm>
#include "tools/macros.hpp"

std::wstring LIA::s2ws(const std::string& str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}
std::string LIA::ws2s(const std::wstring& wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

std::string LIA::trim(std::string s) {
    rtrim(s);
    ltrim(s);
    return s;
}

inline void LIA::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline void LIA::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
std::string LIA::replaceAll(
    const std::string& str,
    const std::string& from,
    const std::string& to
) {
    return str |
     std::views::split(from) |
     std::views::join_with(to) |
     std::ranges::to<std::string>();
};
std::string LIA::sanitizePath(std::string s) {
    s = replaceAll(s, "\\", "/");
    return s;
}

std::string LIA::sanitizeFolderPath(std::string s) {
    s = sanitizePath(s);
    if (!s.ends_with("/")) {
        s = s.append("/");
    }
    return s;
}

std::string LIA::sanitizeFileName(std::string name, std::string extension) {
    if (name.ends_with(extension)) {
        return name;
    }
    std::string namecpy = name;
    namecpy = namecpy.append(extension);
    return namecpy;
}