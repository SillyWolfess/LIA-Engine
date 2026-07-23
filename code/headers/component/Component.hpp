#ifndef LIA_COMPONENT_HPP
#define LIA_COMPONENT_HPP

#include "data/Event.hpp"
#include "tools/macros.hpp"
#include <string>
namespace LIA {
    class ObjectManager;
    class EventManager;
    class ComponentManager;
    class TextureManager;
    class KeybindingManager;
    class Gui;
    class AppWindow;
    enum class ComponentEvent {LOAD, TICK, GET_DATA_GUI, BUTTON_ACTION, INIT_GUI_WINDOW, CHECKBOX_ACTION, OPTION_ACTION, POSITION_CHANGED};
    class Component {
        public:
            bool initCore();
            void setIdentifier(int);
            virtual bool afterInit();
            bool tickEvenHandler(LIA::Event&);
            bool loadEventHandler(LIA::Event&);
            bool getDataGuiEventHandler(LIA::Event&);
            bool buttonActionEventHandler(LIA::Event&);
            bool guiWindowInitHandler(LIA::Event&);
            bool checkboxEventHandler(LIA::Event&);
            bool positionChangedEventHandler(LIA::Event&);
            bool optionChangedHandler(LIA::Event&);
        protected:
            int _identifier = -1;
            virtual bool registerHandlers();
            virtual bool init();
            virtual bool onTick(LIA::Event&);
            virtual bool onLoad(LIA::Event&);
            virtual bool onGetGuiData(LIA::Event&);
            virtual bool onButtonAction(LIA::Event&);
            virtual bool onGuiWindowInit(LIA::Event&);
            virtual bool onCheckboxAction(LIA::Event&);
            virtual bool onPositionChanged(LIA::Event&);
            virtual bool onOptionChanged(LIA::Event&);
            AppWindow* getAppWindow() { return _appWindow;  }
            EventManager* getEventManager() { return _eventManager; }
            ObjectManager* getObjectManager() { return _objectManager; }
            ComponentManager* getComponentManager() { return _componentManager; }
            Gui* getGuiManager() { return _guiManager; }
            TextureManager* getTextureManager() { return _textureManager; }
            KeybindingManager* getKeybindingManager() { return _keybindingManager; }
            bool subscribe(std::string);
            bool subscribe(ComponentEvent);
        private:
            EventManager* _eventManager;
            ObjectManager* _objectManager;
            ComponentManager* _componentManager;
            Gui* _guiManager;
            TextureManager* _textureManager;
            KeybindingManager* _keybindingManager;
            AppWindow* _appWindow;
            bool _registerHandlers();
    };
}
#endif