#ifndef LIA_GUI
#define LIA_GUI

#include "graphics/scene.hpp"
#include "graphics/font.hpp"
#include "data/position.hpp"
#include "data/Padding.hpp"
#include "data/Color.hpp"
#include "window.hpp"
#include "appWindow.hpp"
#include "manager/eventManager.hpp"

#include "watcher/Watcher.hpp"

namespace LIA
{
    class Gui {
        private:
            Watcher _watcher;
            struct Data {
                std::string text;
                Position position;
                Color color;
                int size;
            };
            Style style;
            Scale _lastAppScale;
            bool _lastFullscreen;

            EventManager* _eventManager;
            AppWindow* _appWindow;
            
            std::vector<Data> _data;

            void setDefaults(Data*);
            void setPosition(Data*, float, float);

            std::vector<Window> _windows;
            std::map<std::string, int> _windowMap;

            int _lastGrabbedWindow;

            std::map<int, bool> _keyMap;
            bool registerHandlers();
            
            bool handleGuiButton(Event& event);
//            bool handleCheckbox(Event& event);
            bool handleGuiUpdate(Event& event);
            bool handleEnableEvent(Event& event);
        public:
            ~Gui();
            bool init();
            bool loadStyle();
            void update();

            int add(std::string);
            void passObjects(Scene*, Font*);
            
            void closeWindow(std::string);
            void openWindow(std::string);
            void openWindow(Window*);
            void closeWindow(Window*);
            void toggleWindow(std::string);
            bool loadWindow(std::string name, std::string path);
            /**
             * @param {string} target window
             * @param {string} id
             * @param {string} value
             * @param {string} action
             * @param {string} arg0
             */
            bool registerButton(std::string, std::string, std::string, std::string, std::string);
            Window* getWindow(std::string);
    };
} // namespace LIA
#endif