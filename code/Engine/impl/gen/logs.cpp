#include <iostream>

#include "logs.hpp"
#include "tools.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
std::ofstream LIA::Logger::_file;

void LIA::Logger::log(Level level, std::string msg, std::string file, int line, std::wstring function) {
    std::string prefix = "[INFO] ";
    if (level == Level::ERROR) {
        prefix = "[ERROR] ";
        std::cout << CmdColor::red();
    }
    if (level == Level::WARN) {
        prefix = "[WARN] ";
        std::cout << CmdColor::yellow();
    }
    if (level == Level::FATAL) {
        prefix = "[FATAL] ";
        std::cout << CmdColor::lila();
    }
    if (level == Level::TRACE) {
        prefix = "[TRACE] ";
        std::cout << CmdColor::gray();
    }
    if (level == Level::DEBUG) {
        prefix = "[DEBUG] ";
        std::cout << CmdColor::cyan();
    }
    std::cout << __TIMESTAMP__ << " [" << file << ", " << line << "] " <<  LIA::ws2s(function) << " : " << msg;
    std::cout << CmdColor::white() << std::endl;

    if (!_file.is_open()) {
        _file = std::ofstream("./logs.txt");//, std::ios_base::app);
    }
    if (_file.is_open()) {
        _file << prefix << __TIMESTAMP__ << " [" << file << ", " << line << "] " <<  LIA::ws2s(function) << " : " << msg << "\n";
        _file.flush();
    }
//    _file.close();
}