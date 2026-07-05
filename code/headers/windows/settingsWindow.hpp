#ifndef LIA_SETTINGS_WINDOW
#define LIA_SETTINGS_WINDOW

#include "component/BaseGame.hpp"

namespace LIA {
    class SettingsWindow : public BaseGame {
    protected:
        bool init();
        virtual bool registerHandlers();
        virtual bool onGetGuiData(LIA::Event&);
        virtual bool onCheckboxAction(LIA::Event&);
        //            virtual bool onButtonAction(LIA::Event&);
        //            virtual bool onGuiWindowInit(LIA::Event&);
    private:
        std::string _eventSource;
        void updateData(std::string);
        void handleCheckbox(LIA::Event&);
    };
}
#endif