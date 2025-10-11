#ifndef LIA_CAMERA
#define LIA_CAMERA
#include "precompiled.hpp"
#include "data/position.hpp"

namespace LIA {
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
        public: 
           bool init();
           void setLocked(bool locked) { _locked = locked; };
           bool isLocked() { return _locked; };
           void update(float, float);
//           void destroy();
           void switchOrtho();
           void swtichPersp();

           void move(Position&);

           glm::mat4 getProjection();
           glm::mat4 getView();
           Position& getPosition() { return _position; };
           Position& getLookAt() { return _lookAt; };
           void setPosition(Position& position) { copy(_position, position); };
           ~Camera();
    };
}
#endif