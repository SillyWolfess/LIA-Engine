#ifndef LIA_TERRAIN
#define LIA_TERRAIN

#include "precompiled.hpp"
#include "data/Model.hpp"
#include "data/position.hpp"
#include <vector>

namespace LIA {
    class Scene;
    class Terrain {
        public:
            Terrain();
            void setEnabled(bool value) { _enabled = value; }
            bool isEnabled() { return _enabled; }
            bool load();
            bool pass(Scene*);
            void setWire(bool);
            float heightAt(int, int);
            ~Terrain();
        private:
            int _sizeX;
            int _sizeY;
            bool _isInGpu;
            float _roughness;
            float _scale;
            
            float _maxHeight;
            float _maxDepth;
            
            bool _wireMode;
            bool _drawAsPoints;
            bool _enabled;

            Rotation _rotation;
            Model _model;
            std::string _shader;
            GLint _vao;
        private:
            bool generate(int, int);
            void applyMidpointDisplacement();
            bool sendToGpu();
            void makeIndices();
            void diamondStep(int rectSize, float curheight);
            void squareStep(int rectSize, float curheight);
            float randomFloatRange(float a, float b);
            void setAt(int, int, float);
            void normalize(float, float);
    };
}
#endif