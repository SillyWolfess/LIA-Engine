#ifndef LIA_KEYBINDING_MANAGER_HPP
#define LIA_KEYBINDING_MANAGER_HPP
#include <string>
#include <map>
#include "watcher/Watcher.hpp"

namespace LIA {
    class KeybindingControls {
        private:
            /**
             * @param {string} action name
             * @param {int} key
             */
            std::map<std::string, int> _map;
        public:
            void set(std::string, int);
            int get(std::string);
            void clear() { _map.clear(); }
    };

    class KeybindingManager {
        public:
            KeybindingManager();
            bool init();
            /***
             * registers file with controls, if exists, overwrites the target
             * 
             * does not watch for file changes
             * @param {string} name
             * @param {string} file path
             */
            bool registerControls(std::string, std::string);
            /***
             * registers file with controls, if exists, overwrites the target
             * @param {string} name
             * @param {string} file path
             * @param {bool} watch for changes and reload
             */
            bool registerControls(std::string, std::string, bool);
            /***
             * loads controls from the registered file, else return false
             * @param {string} name
             */
            bool load(std::string);
            /***
             * @param {string} name
             */
            KeybindingControls& getControls(std::string);
            /**
             * call to update file watching
             */
            bool update();
            ~KeybindingManager();
        private:
            Watcher _watcher;
            std::map<std::string, KeybindingControls> _controls;
            std::map<std::string, std::string> _pathMap;
    };
}
#endif