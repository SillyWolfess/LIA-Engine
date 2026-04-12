#ifndef LIA_KEY_MAPPING_WINDOW
#define LIA_KEY_MAPPING_WINDOW

#include "component/BaseGame.hpp"

namespace LIA {
    class KeyMappingWindow : public BaseGame {
         protected:
            bool init();
            virtual bool registerHandlers();
            virtual bool onGetGuiData(LIA::Event&);
//            virtual bool onButtonAction(LIA::Event&);
//            virtual bool onGuiWindowInit(LIA::Event&);
        private:
            std::string _eventSource;
            void updateData(std::string);
    };
}
#endif