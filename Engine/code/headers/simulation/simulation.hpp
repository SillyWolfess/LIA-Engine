#ifndef LIA_SIMULATION
#define LIA_SIMULATION

#include "graphics/scene.hpp"
#include "graphics/font.hpp"
#include "manager/objectManager.hpp"
#include "manager/shaderManager.hpp"
#include "manager/eventManager.hpp"
#include "appWindow.hpp"

#include "simulation/physics.hpp"
#include "simulation/movement.hpp"

#include "data/tSimState.hpp"

namespace LIA {
    class Simulation {
        private:
            LIA_SIM_STATE _state = LIA_SIM_STATE::EMPTY;
            LIA_GAME_STATE _gameState = LIA_GAME_STATE::RUNNING;
            ObjectManager* _objManager;
            EventManager* _eventManager;
            Physics _physics;
            Movement _movement;

            std::vector<Light> _lights;
            std::vector<int> _lightIds;
            bool loadObject(std::string path);
            bool loadLight(std::string path);
            int _playerId = -1;
        public:
            ~Simulation();
            
            bool init();
            bool load(ShaderManager*);
            bool update(AppWindow*, float);
            int getPlayerId() { return _playerId; };

            void passObjects(Scene*);
            
            void pause() {
                if (_state != LIA_SIM_STATE::RUNNING) {
                    return;
                }
                _state = LIA_SIM_STATE::PAUSED;
            }
            void end() {
                if (_state == LIA_SIM_STATE::ENDED) {
                    return;
                }
                _state = LIA_SIM_STATE::ENDED;
            }
            void unpause() { 
                if (_state != LIA_SIM_STATE::PAUSED && _state != LIA_SIM_STATE::LOADED) {
                    return;
                }
                _state = LIA_SIM_STATE::RUNNING;
            }
            LIA_SIM_STATE getState() {
                return _state;
            }
            bool isWon() { return _gameState == LIA_GAME_STATE::WON; }
            bool isLost() { return _gameState == LIA_GAME_STATE::LOST; }
            bool isProgress() { return !isWon() && !isLost(); }
            void startLoading() { _state = LIA_SIM_STATE::LOADING; }
            bool isLoading() { return _state == LIA_SIM_STATE::LOADING; }
            void setWon() { _gameState = LIA_GAME_STATE::WON; }
            void setLost() { _gameState = LIA_GAME_STATE::LOST; }
            void resetWinLos() { _gameState = LIA_GAME_STATE::RUNNING; }
            void reset();
    };
}

#endif