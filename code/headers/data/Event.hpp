#ifndef LIA_EVENT
#define LIA_EVENT
#include <string>
#include <vector>
namespace LIA {
    enum class EventType {ANY, GUI, SIMULATION};
    struct Event {
        EventType type;
        std::string subtype;
        std::string source;
        std::string name;
        std::string action;
        std::string window;
        std::string target;
        std::string arg0;
        int argi;
        bool argb;
        bool useList = false;
        std::vector<std::string> argl;
    };
    struct FpsEvent : Event {
        FpsEvent() = delete;
        FpsEvent(int fps) {
            action = "fps";
            source = "fps";
            name = "fps";
            argi = fps;
            arg0 = "";
            type = EventType::GUI;
        }
    };
    struct PositionChangedEvent : Event {
        PositionChangedEvent() = delete;
        PositionChangedEvent(std::string source) {
            this->type = EventType::SIMULATION;
            this->source = source;
            this->name = "position_changed";
        }
    };
    struct LoadEvent : Event {
        LoadEvent() = delete;
        LoadEvent(std::string source, int id) {
            this->type = EventType::SIMULATION;
            this->argi = id;
            this->name = "load";
            this->source = source;
        }
    };
    struct GameLostEvent : Event {
        GameLostEvent() {
            this->type = EventType::SIMULATION;
            this->name = "gameLost";
            this->arg0 = "end_menu";
        }
        GameLostEvent(std::string windowToOpen) {
            this->type = EventType::SIMULATION;
            this->name = "gameLost";
            this->arg0 = windowToOpen;
        };
    };
    struct GameWonEvent : Event {
        GameWonEvent() {
            this->type = EventType::SIMULATION;
            this->name = "gameWon";
            this->arg0 = "end_menu";
        }
        GameWonEvent(std::string windowToOpen) {
            this->type = EventType::SIMULATION;
            this->name = "gameWon";
            this->arg0 = windowToOpen;
        }
    };
    struct TickEvent : Event {
        TickEvent() = delete;
        TickEvent(std::string source) {
            this->type = EventType::SIMULATION;
            this->name = "tick";
            this->source = source;
        }
    };
    /***
    * @param {string} source
    * @param {string} action
    * @param {string} argument
    * @param {string} window (optional)
    */
    struct ButtonEvent: Event {
        ButtonEvent() = delete;
        /***
         * @param {string} source
         * @param {string} action
         * @param {string} argument
         */
        ButtonEvent(std::string source, std::string action, std::string arg0) {
            this->name = "button_action";
            this->type = EventType::GUI;
            this->source = source;
            this->action = action;
            this->arg0 = arg0;
        }
        /***
         * @param {string} source
         * @param {string} action
         * @param {string} argument
         * @param {string} window
         */
        ButtonEvent(std::string source, std::string action, std::string arg0, std::string window) {
            this->name = "button_action";
            this->type = EventType::GUI;
            this->source = source;
            this->action = action;
            this->arg0 = arg0;
            this->window = window;
        }
    };
    struct CheckBoxEvent: Event {
        CheckBoxEvent() = delete;
        CheckBoxEvent(std::string source, std::string action) {
            this->name = "checkbox_action";
            this->type = EventType::GUI;
            this->source = source;
            this->action = action;
        }
    };
    struct OptionSelectionChangeEvent: Event {
        OptionSelectionChangeEvent() = delete;
        OptionSelectionChangeEvent(std::string window, std::string action, int index) {
            this->name = "option_action";
            this->type = EventType::GUI;
            this->source = window;
            this->window = window;
            this->action = action;
            this->argi = index;
        }
    };
    //UpdateGuiEvent updateGuiEvent("settings", "fullscreen", _glfwSettings.maximize);
    struct UpdateGuiEvent: Event {
        UpdateGuiEvent() = delete;
        UpdateGuiEvent(std::string window, std::string target, bool value) {
            name = "update_gui";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            arg0 = value ? "true" : "false";
        }
        UpdateGuiEvent(std::string window, std::string target, int value) {
            name = "update_gui";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            arg0 = std::to_string(value);
        }
        UpdateGuiEvent(std::string window, std::string target, std::string value) {
            name = "update_gui";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            arg0 = value;
        }
    };
    struct UpdateGuiStringEvent: Event {
        UpdateGuiStringEvent() = delete;
        UpdateGuiStringEvent(std::string window, std::string target, std::string value) {
            name = "update_gui";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            arg0 = value;
        }
    };
    struct UpdateGuiListEvent: Event {
        UpdateGuiListEvent() = delete;
        UpdateGuiListEvent(std::string window, std::string target, std::vector<std::string> &value) {
            name = "update_gui";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            this->useList = true;
            argl = value;
            argi = -1;
        };
        UpdateGuiListEvent(std::string window, std::string target, std::vector<std::string> &value, int subtype) {
            name = "update_gui";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            this->useList = true;
            argl = value;
            argi = subtype;
        };
    };
    struct SetGridFieldEnabledEvent: Event {
        SetGridFieldEnabledEvent() = delete;
        SetGridFieldEnabledEvent(std::string window, std::string parent, std::string target, bool value) {
            name = "set_enabled";
            type = EventType::GUI;
            subtype = "grid";
            this->window = window;
            this->target = parent;
            this->arg0 = target;
            this->argb = value;
        };
    };
    struct SetGuiEnabledEvent: Event {
        SetGuiEnabledEvent() = delete;
        SetGuiEnabledEvent(std::string window, std::string target, bool value) {
            name = "set_enabled";
            subtype = "none";
            type = EventType::GUI;
            this->window = window;
            this->target = target;
            argb = value;
        }
    };
    struct GuiGetDataEvent: Event {
        GuiGetDataEvent() = delete;
        GuiGetDataEvent(std::string window) {
            name = "get_data_gui";
            type = EventType::GUI;
            this->window = window;
            this->source = window;
        };
    };
    struct GuiInitEvent: Event {
        GuiInitEvent() = delete;
        GuiInitEvent(std::string window) {
            name = "init_gui_window";
            type = EventType::GUI;
            this->window = window;
            this->source = window;
        };
    };
}
#endif