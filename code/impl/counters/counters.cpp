#include "counters/counters.hpp"
#include "logs.hpp"

std::map<std::string, int> LIA_DEBUG::Counters::_constructorCalls;
std::map<std::string, int> LIA_DEBUG::Counters::_constructorCopyCalls;
std::map<std::string, int> LIA_DEBUG::Counters::_destructorCalls;
std::map<std::string, int> LIA_DEBUG::Counters::_diffCalls;
std::map<std::string, int> LIA_DEBUG::Counters::_maxCount;

void LIA_DEBUG::Counters::ensureExists(std::string name) {
    if (_constructorCalls.find(name) == _constructorCalls.end()) {
        _constructorCalls.emplace(std::pair<std::string, int>(name, 0));
    }
    if (_constructorCopyCalls.find(name) == _constructorCopyCalls.end()) {
        _constructorCopyCalls.emplace(std::pair<std::string, int>(name, 0));
    }
    if (_destructorCalls.find(name) == _destructorCalls.end()) {
        _destructorCalls.emplace(std::pair<std::string, int>(name, 0));
    }
    if (_diffCalls.find(name) == _diffCalls.end()) {
        _diffCalls.emplace(std::pair<std::string, int>(name, 0));
    }
}

void LIA_DEBUG::Counters::constructor(std::string name, bool log) {
    if (log) {
        LIA_debug(std::vformat("{}()", std::make_format_args(name)));
    }
    ensureExists(name);
    _constructorCalls[name] = _constructorCalls[name] + 1;
    _diffCalls[name] = _constructorCalls[name] - _destructorCalls[name];
}

void LIA_DEBUG::Counters::destructor(std::string name, bool log) {
    if (log) {
        LIA_debug(std::vformat("~{}()", std::make_format_args(name)));
    }
    ensureExists(name);
    _destructorCalls[name] = _destructorCalls[name] + 1;
    _diffCalls[name] = _constructorCalls[name] - _destructorCalls[name];
}

void LIA_DEBUG::Counters::constructorCopy(std::string name, bool log) {
    if (log) {
        LIA_debug(std::vformat("{}(const &{})", std::make_format_args(name, name)));
    }
    ensureExists(name);
    _constructorCalls[name] = _constructorCalls[name] + 1;
    _diffCalls[name] = _constructorCalls[name] - _destructorCalls[name];
    _constructorCopyCalls[name] = _constructorCopyCalls[name] + 1;
}

void LIA_DEBUG::Counters::maxCounter(std::string name, int count) {
    if (_maxCount.find(name) == _maxCount.end()) {
        _maxCount.emplace(std::pair<std::string, int>(name, 0));
    }
    _maxCount[name] = count > _maxCount[name] ? count : _maxCount[name];
}

void LIA_DEBUG::Counters::log() {
    for (auto [name, count] : _maxCount) {
        LIA_debug(std::vformat("{} = {}", std::make_format_args(name, count)));
    }
    for (auto [name, count] : _constructorCalls) {
        if (_constructorCopyCalls[name]) {
            LIA_warn(std::vformat("{} = {} constructors ({} copies)", std::make_format_args(name, count, _constructorCopyCalls[name])));
        } else {
            LIA_debug(std::vformat("{} = {} constructors", std::make_format_args(name, count)));
        }
    }
    for (auto [name, count] : _destructorCalls) {
        LIA_debug(std::vformat("{} = {} destructors", std::make_format_args(name, count)));
    }
    for (auto [name, count] : _diffCalls) {
        if (count != 0) {
            /*
            LIA_fatal(std::vformat("{} has difference in constructor / destructor by {}", std::make_format_args(name, count)));
            LIA_fatal(std::vformat("{} = {} constructors total", std::make_format_args(name, _constructorCalls[name])));
            LIA_fatal(std::vformat("{} = {} copy constructors", std::make_format_args(name, _constructorCopyCalls[name])));
            LIA_fatal(std::vformat("{} = {} destructors", std::make_format_args(name, _destructorCalls[name])));
            */
            LIA_fatal_f("{} has difference in constructor / destructor by {}", name, count);
            LIA_fatal_f("{} = {} constructors total", name, _constructorCalls[name]);
            LIA_fatal_f("{} = {} copy constructors", name, _constructorCopyCalls[name]);
            LIA_fatal_f("{} = {} destructors", name, _destructorCalls[name]);
        }
    }
}