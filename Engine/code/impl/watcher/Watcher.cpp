#include "watcher/Watcher.hpp"
#include "logs.hpp"
#include "tools/macros.hpp"

void LIA::Watcher::watch() {
    for (auto& [fileName, watchList] : _handlers) {
        bool wasChanged = needsReload(fileName);
        if (wasChanged) {
            LIA_trace_f("Calling handlers for {}", fileName);
            for (WatcherHandlers& watcher: watchList) {
                watcher.handler();
            }
        }
    }
}

void LIA::Watcher::subscribe(std::string fileName) {
    LIA_TRY
        if (!registerFile(fileName)) {
            LIA_error_f("Failed to register watcher for '{}'", fileName);
        }
    LIA_CATCH_EMPTY
}

void LIA::Watcher::subscribe(std::string fileName, std::function<bool()> handler) {
    LIA_TRY
        if (!registerFile(fileName)) {
            LIA_error_f("Failed to register watcher for '{}'", fileName);
            return;
        }
        LIA_debug_f("Adding watcher handlers for {} ", fileName);
        if (_handlers.find(fileName) == _handlers.end()) {
            std::list<WatcherHandlers> handlersList;
            _handlers.emplace(std::pair<std::string, std::list<WatcherHandlers>>(fileName, handlersList));
        }
        std::list<WatcherHandlers>& handlers = _handlers[fileName];
        WatcherHandlers& ev = handlers.emplace_back();
        ev.handler = handler;
    LIA_CATCH_EMPTY
}

bool LIA::Watcher::registerFile(std::string fileName) {
     if(!std::filesystem::is_regular_file(std::filesystem::path(fileName))) {
        LIA_error_f("'{}' is not regular file", fileName);
        return false;
    }
    _paths[fileName] =  std::filesystem::last_write_time(std::filesystem::path(fileName));
    LIA_debug_f("'{}' added to watchers", fileName);
    return true;
}

bool LIA::Watcher::needsReload(std::string fileName) {
    auto lastWriteTime = std::filesystem::last_write_time(fileName);
    if (_paths.find(fileName) == _paths.end()) {
       return false;
    } else if(_paths[fileName] != lastWriteTime) { 
       _paths[fileName] = lastWriteTime;
       LIA_debug_f("'{}' was changed", fileName);
       return true;
    }
    return false;
}