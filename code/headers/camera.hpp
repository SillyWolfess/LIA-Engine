#ifndef LIA_CAMERA
#define LIA_CAMERA
#include "precompiled.hpp"
#include "data/position.hpp"
#include "watcher/Watcher.hpp"
#include "manager/keybindingManager.hpp"

namespace LIA {
    class EventManager;
    class Camera {
        private:
            struct s_persp_settings {
                float near;
                float far;
                float fov;
                float res;
            } _perspSettings;
            
            struct s_ortho_settings {
                float near;
                float far;
                float w;
                float h;
            } _orthoSettings;

            bool _ortho;
            bool _locked;

            Position _position;
            Position _worldDirection;
            Position _lookAt;

            struct s_settings {
                bool locked;
                bool ortho;
                Position position;
                float step;
                std::string controls;
            } _settings;
            KeybindingControls _cameraControls;
            Watcher _watcher;
        public: 
           bool init();
           bool loadFromSettings(std::string);
           bool loadControls(std::string);
           void setLocked(bool locked) { _locked = locked; };
           bool isLocked() { return _locked; };
           void update(float, float);
           void update(EventManager*);
//           void destroy();
           void switchOrtho();
           void swtichPersp();

           void move(Position&);

           glm::mat4 getProjection();
           glm::mat4 getView();
           Position& getPosition() { return _position; };
           Position& getLookAt() { return _lookAt; };
           void enableControls() { _cameraControls.enable(); }
           void disableControls() { _cameraControls.disable(); }
           void setPosition(Position& position) { copy(_position, position); };
           void onPositionChanged(EventManager*);
           ~Camera();
    };
}
#endif