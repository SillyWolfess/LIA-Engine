#include "Engine.hpp"
#include "test/TestGame.hpp"
#include "component/BaseGame.hpp"

int main() {
    LIA::Engine& engine = LIA::Engine::getInstance();
    TEST_GAME::TestGame game;
    engine.setGame(&game);
    engine.run();
    return 0;
}