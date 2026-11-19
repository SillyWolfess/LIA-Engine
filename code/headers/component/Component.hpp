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
    class AnimationManager;
    class Engine;
    enum class ComponentEvent {LOAD, TICK, GET_DATA_GUI, BUTTON_ACTION, INIT_GUI_WINDOW, CHECKBOX_ACTION, OPTION_ACTION, POSITION_CHANGED, TIMER, DRAW_PASS};
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
            bool timerHandler(LIA::Event&);
            bool drawPassEventHandler(LIA::Event&);
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
            virtual bool onTimer(LIA::Event&);
            virtual bool onDrawPass(LIA::Event&);

            AppWindow& getAppWindow();
            EventManager& getEventManager();
            ObjectManager& getObjectManager();
            ComponentManager& getComponentManager();
            Gui& getGuiManager();
            TextureManager& getTextureManager();
            KeybindingManager& getKeybindingManager();
            AnimationManager& getAnimationManager();

            bool subscribe(std::string);
            bool subscribe(ComponentEvent);
        private:
            Engine *_engine;
            bool _registerHandlers();
    };
}
#endif