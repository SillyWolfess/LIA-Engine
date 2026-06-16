#ifndef TEST_GAME_HPP
#define TEST_GAME_HPP
#include <component/BaseGame.hpp>
#include <string>

namespace TEST_GAME {
    class TestGame : public LIA::BaseGame {
        protected:
            bool init();
            virtual bool registerHandlers();
            virtual bool onGetGuiData(LIA::Event&);
            virtual bool onGuiWindowInit(LIA::Event&);
            virtual bool onLoad(LIA::Event&);
        private:
            std::string _eventSource;
            void updateData(std::string);
    };
}
#endif