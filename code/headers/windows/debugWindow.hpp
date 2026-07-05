#ifndef LIA_DEBUG_WINDOW
#define LIA_DEBUG_WINDOW

#include "component/BaseGame.hpp"

namespace LIA {
    class DebugWindow : public BaseGame {
    protected:
        bool init();
        virtual bool registerHandlers();
        virtual bool onGetGuiData(LIA::Event&);
    private:
        std::string _eventSource;
        void updateData(std::string);
    };
}
#endif