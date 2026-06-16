#ifndef LIA_ENGINE
#define LIA_ENGINE

#include "appWindow.hpp"
#include "simulation/simulation.hpp"
#include "terrain/Terrain.hpp"
#include "gui/gui.hpp"
#include "manager/shaderManager.hpp"
#include "manager/eventManager.hpp"
#include "manager/componentManager.hpp"
#include "manager/objectManager.hpp"
#include "manager/textureManager.hpp"
#include "manager/materialManager.hpp"
#include "manager/keybindingManager.hpp"
#include "component/BaseGame.hpp"

#include "watcher/Watcher.hpp"
#include "counters/FpsCounter.hpp"

#include "windows/keyMappingWindow.hpp"

namespace LIA
{
    class Engine {
        private:
            Watcher _watcher;
            AppWindow _window;
            Simulation _simulation;
            Gui _gui;
            ShaderManager _shaderManager;
            EventManager _eventManager;
            ComponentManager _componentManager;
            ObjectManager _objectManager;
            TextureManager _textureManager;
            MaterialManager _materialManager;
            KeybindingManager _keybindingManager;
            Terrain _terrain;
            FpsCounter _fpsCounter;
            BaseGame* _game = nullptr;

            KeyMappingWindow _keyMappingWindow;

            bool registerEventHandlers();
            bool handleGui(Event& event);
            bool handleGuiGetData(Event& event);
            bool handleGameState(Event&);
            bool exit();

            bool init();
            bool initEngineComponents();
            bool load();
            bool update();
            void draw();
            bool isExit();
        public:
            Engine();
            bool initImplementation(Component*);
            void run();
            void fatal() { _window.close(); }
            ShaderManager& getShaderManager() { return _shaderManager; }
            EventManager& getEventManager() { return _eventManager; }
            ComponentManager& getComponentManager() { return _componentManager; }
            ObjectManager& getObjectManager() { return _objectManager; }
            TextureManager& getTextureManager() { return _textureManager; }
            MaterialManager& getMaterialManager() { return _materialManager; }
            KeybindingManager& getKeybindingManager() { return _keybindingManager; }
            Gui& getGuiManager() { return _gui; }
            AppWindow& getAppWindow() { return _window; }
            Camera& getMainCamera() { return _window.getCamera(); }
            Terrain& getTerrain() { return _terrain; }
            Scene* getTerrainRenderer() { return _window.getTerrain(); }
            ~Engine();
            void setGame(BaseGame* game);
            static Engine& getInstance();
    };
} // namespace LIA
#endif