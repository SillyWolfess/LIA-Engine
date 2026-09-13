#ifndef LIA_APP_WINDOW
#define LIA_APP_WINDOW

#include "precompiled.hpp"
#include "graphics/scene.hpp"
#include "graphics/font.hpp"
#include "camera.hpp"
#include "manager/shaderManager.hpp"
#include "data/position.hpp"
#include "controllers/InputController.hpp"
#include "controllers/MouseController.hpp"
#include "manager/eventManager.hpp"

namespace LIA
{
    class AppWindow {
        public:
            struct s_glfwSettings {
                int major;
                int minor;
                bool compact;
                int samples;

                bool experimental;
                bool maximize;
                bool fullscreen;
                bool vSync;

                int width;
                int height;

                int windowMode;
                bool enableResize;
                bool enableMaximize;
            } _glfwSettings;

            struct s_windowModes {
                int width;
                int height;
            };

            std::vector<s_windowModes> _supportedResolutions;

            static KeyController _keyController;
            static MouseController _mouseController;
            static int _cpX;
            static int _cpY;
            static int _offsetCpX;
            static int _offsetCpY;
            static int _buttonEvent;

            static int getButtonEvent() { return _buttonEvent; }
            static void setButtonEvent(int buttonEvent) { _buttonEvent = buttonEvent; }

            static void calculateOffset(double x, double y) {         
                _offsetCpX = x - _cpX;
                _offsetCpY = y - _cpY;
            }

            static void resetOffset() {
                _offsetCpX = 0;
                _offsetCpY = 0;
            }

            static void setCp(int x, int y) {
                _cpX = x;
                _cpY = y;
            }

            static void mouseClick(int button, float x, float y) {
                _mouseController.setMouseClick(button, x, y);
            }

            static void mouseRelease(int button, float x, float y) {
                _mouseController.setMouseRelease(button, x, y);
            }

            static int getCpX() { return _cpX; }
            static int getCpY() { return _cpY; }
            static int getOffsetCpX() { return _offsetCpX; }
            static int getOffsetCpY() { return _offsetCpY; }

            static void keyPressed(int keyCode) { _keyController.setKeyPressed(keyCode); }
            static void keyReleased(int keyCode) { _keyController.setKeyReleased(keyCode); }
            static bool isKeyPressed(std::string keyGroup ,int keyCode) { return _keyController.isKeyPressed(keyGroup, keyCode); }
            static int lastKeyPressed() { return _keyController.getLastKeyPressed(); }
            static std::string getKeyName(int keyCode) { return _keyController.getKeyName(keyCode); }

            static bool isMousePressed() { return _mouseController.isMouseClicked(); }
            static bool wasMouseClicked() { return _mouseController.wasMouseClicked(); }
            static bool isMouseHeld() { return _mouseController.isHeld(); }

            static bool isRMousePressed() { return _mouseController.isRMouseClicked(); }
            static bool wasRMouseClicked() { return _mouseController.wasRMouseClicked(); }
            static bool isRMouseHeld() { return _mouseController.isRHeld(); }

            static void resetState() { _mouseController.resetState(); }

        //    static void windowResized(int w, int h) { windowW = w; windowH = h; };
        //    static int windowW, windowH;
       private:
            EventManager* _eventManager;
            /*
            struct s_background {
                float r, g, b, a;
            } _background;
            */
            Color _background;
            bool _guiDepthTest;

            GLFWwindow* _glfwWindow;
            GLFWcursor* _cursor;
            int windowWold, windowHold;
            int windowW, windowH;
            int windowLastX, windowLastY;
            
            void loadSettings();
            void loadResolutions();
            void handleMouseInput();
//            bool handleCheckbox(Event& event);
//            bool handleGuiGetData(Event& event);
//            bool registerHandlers(EventManager*);

            bool _closeThis;

            Scene _scene;
            Scene _gui;
            Scene _terrain;
            Font _font;
            Camera _mainCamera;
            Camera _guiCamera;

            void onCameraPositionChanged();
        public:
            ~AppWindow();
            bool init(std::string);
            bool initPrefab();
            bool initFont(/*GLuint*/);
            void update();
            void draw();
            void close();
            bool isExit();
            
            Scene* getScene() { return &_scene; }
            Scene* getGui() { return &_gui; }
            Scene* getTerrain() { return &_terrain; }
            Font* getFont() { return &_font; }
            Camera& getCamera() { return _mainCamera; }

            bool isCursorInsideWindow();
            bool isKeyPressedPoll(int);
            bool isKeyReleasedPoll(int);
            Position getMousePos();
            Scale getWindowScale();
            s_glfwSettings getSettings() { return _glfwSettings;  }
            std::vector<s_windowModes> getResolutions() { return _supportedResolutions; }
            bool isFullscreen() { return _glfwSettings.fullscreen; }
            void setMode(int);
            void toogleFullscreen();
            void resize(int, int);
    };
}
#endif