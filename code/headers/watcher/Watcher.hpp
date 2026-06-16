#ifndef LIA_WATCHER
#define LIA_WATCHER

#include <map>
#include <unordered_map>
#include <list>
#include <string>
#include <functional>
#include <filesystem>

namespace LIA {
    class Watcher {
        public:
            void subscribe(std::string, std::function<bool()> handler);
            void subscribe(std::string);
            bool needsReload(std::string);
            void watch();
        private:
            struct WatcherHandlers {
                std::function<bool()> handler;
            };
            std::map<std::string, std::list<WatcherHandlers>> _handlers;
            std::unordered_map<std::string, std::filesystem::file_time_type> _paths;
        private:
            bool registerFile(std::string);
    };
}
#endif