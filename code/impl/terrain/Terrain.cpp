#include "logs.hpp"
#include "terrain/Terrain.hpp"

#include "graphics/VertexArrayObject.hpp"
#include "graphics/VertexBuffer.hpp"
#include "graphics/scene.hpp"
#include "data/position.hpp"
#include "graphics/glwrapper.hpp"

#include "loaders/xmlLoader.hpp"

#include "Engine.hpp"

#include <GL/glew.h>
#include <gl/gl.h>
#include <glm/glm.hpp>

bool LIA::Terrain::load() {
    if (!_enabled) {
        LIA_debug("Terrain is disabled. Skipping loading.");
        return true;
    }
    if (_isInGpu) {
        LIA_trace("Terrain is already in gpu. Skipping loading.");
        return true;
    }
    XmlLoader xmlLoader;
    XmlLoader::XmlData terrainData = xmlLoader.load("./data/terrain/terrain.xml");
    Scale scale = xmlLoader.getScale2D(terrainData, "size");
    _scale = xmlLoader.getFloat(terrainData, "scale");
    _roughness = xmlLoader.getFloat(terrainData, "roughness");
    _maxHeight = xmlLoader.getFloat(terrainData, "maxHeight");
    _maxDepth = xmlLoader.getFloat(terrainData, "maxDepth");
    _wireMode = xmlLoader.getBoolean(terrainData, "wireMode");
    _drawAsPoints = xmlLoader.getBoolean(terrainData, "drawAsPoints", false);
    if (!generate(scale.x, scale.x)) {
        LIA_fatal("Failed to generate terrain");
        return false;
    }
    if (!sendToGpu()) {
        LIA_fatal("Failed to send terrain data to gpu");
        return false;
    }
    return true;
}

LIA::Terrain::Terrain() : _isInGpu{false}, _enabled{false} {
    _shader = std::string{"terrain"};
    _rotation = emptyRotation();
}

LIA::Terrain::~Terrain() {

}

bool LIA::Terrain::generate(int sizeX, int sizeY) {
    _sizeX = sizeX;
    _sizeY = sizeY;
    for (int y = 0; y < _sizeY; y++) {
        for (int x = 0; x < _sizeX; x++) {
            glm::vec3 point;
            point.x = x * _scale;
            point.y = 0.0f;
            point.z = y * _scale;
            _model.data.vertices.push_back(point);
        }
    }
    
    _model.size = _model.data.vertices.size();
    _model.hasIndices = false;

    applyMidpointDisplacement();
    normalize(_maxDepth, _maxHeight);

    makeIndices();
    return true;
}

void LIA::Terrain::normalize(float min, float max) {
    float rMin = 10000;
    float rMax = -10000;
    for (glm::vec3 &point : _model.data.vertices) {
        if (point.y > rMax) {
            rMax = point.y;
        }
        if (point.y < rMin) {
            rMin = point.y;
        }
    }
    if (rMax <= rMin) {
        return;
    }
    float delta = rMax - rMin;
    float range = max - min;
    LIA_trace_f("Normalizing from [{}, {}] to [{}, {}]", rMin, rMax, min, max);

    for (glm::vec3 &point : _model.data.vertices) {
        point.y = ((point.y - rMin) / delta) * range + min;
    }
    
    rMin = 10000;
    rMax = -10000;
    for (glm::vec3 &point : _model.data.vertices) {
        if (point.y > rMax) {
            rMax = point.y;
        }
        if (point.y < rMin) {
            rMin = point.y;
        }
    }
    LIA_trace_f("New range [{}, {}]", rMin, rMax);
}

int calcNextPowerOfTwo(int n) {
    unsigned long long p = 1;
    if (n && !(n & (n - 1))) 
        return n;
    while (p < n)
        p <<= 1;
    return p;
}

void LIA::Terrain::applyMidpointDisplacement() {
    int rectSize = calcNextPowerOfTwo(_sizeX);
    float curHeight = (float)rectSize / 2.0f;
    float heightReduce = std::pow(2.0f, -_roughness);

    while (rectSize > 0) {
        diamondStep(rectSize, curHeight);
        squareStep(rectSize, curHeight);

        rectSize /= 2;
        curHeight *= heightReduce;
    }
}

float LIA::Terrain::heightAt(int x, int y) {
    if (!(x > -1 && x < _sizeX)) {
        LIA_fatal_f("x {} is out of bounds for [0, {}]", x, _sizeX);
    }
    if (!(y > -1 && y < _sizeY)) {
        LIA_fatal_f("y {} is out of bounds for [0, {}]", y, _sizeY);
    }
    LIA_ASSERT(x > -1 && x < _sizeX, "x is out of bounds");
    LIA_ASSERT(y > -1 && y < _sizeY, "y is out of bounds");
    return _model.data.vertices[(x * _sizeX) + y].y;
}

void LIA::Terrain::setAt(int x, int y, float value) {
    if (!(x > -1 && x < _sizeX)) {
        LIA_fatal_f("x {} is out of bounds for [0, {}]", x, _sizeX);
    }
    if (!(y > -1 && y < _sizeY)) {
        LIA_fatal_f("y {} is out of bounds for [0, {}]", y, _sizeY);
    }
    LIA_ASSERT(x > -1 && x < _sizeX, "x is out of bounds");
    LIA_ASSERT(y > -1 && y < _sizeY, "y is out of bounds");
    _model.data.vertices[(x * _sizeX) + y].y = value;
}

void LIA::Terrain::diamondStep(int rectSize, float curheight) {
    int halfRectSize = rectSize / 2;
    for (int y = 0; y < _sizeY; y += rectSize) {
        for (int x = 0; x < _sizeX; x += rectSize) {
            int nextX = (x + rectSize) % _sizeX;
            int nextY = (y + rectSize) % _sizeY;
            
            if (nextX < x) {
                nextX = _sizeX - 1;
            }
            if (nextY < y) {
                nextY = _sizeY - 1;
            }

            float tl = heightAt(x, y);
            float tr = heightAt(nextX, y);
            float bl = heightAt(x, nextY);
            float br = heightAt(nextX, nextY);

            int midX = (x + halfRectSize) % _sizeX;
            int midY = (y + halfRectSize) % _sizeY;

            float randValue = randomFloatRange(-curheight, curheight);
            float midPoint = (tl + tr + bl + br) / 4.0f;
            setAt(midX, midY, midPoint + randValue);
        }
    }
}

void LIA::Terrain::squareStep(int rectSize, float curheight) {
    int halfRectSize = rectSize / 2;
    for (int y = 0; y < _sizeY; y += rectSize) {
        for (int x = 0; x < _sizeX; x += rectSize) {
            int nextX = (x + rectSize) % _sizeX;
            int nextY = (y + rectSize) % _sizeY;
            
            if (nextX < x) {
                nextX = _sizeX - 1;
            }
            if (nextY < y) {
                nextY = _sizeY - 1;
            }

            int midX = (x + halfRectSize) % _sizeX;
            int midY = (y + halfRectSize) % _sizeY;

            int prevX = (x - halfRectSize + _sizeX) % _sizeX;
            int prevY = (y - halfRectSize + _sizeY) % _sizeY;

            float ctl = heightAt(x, y);
            float ctr = heightAt(nextX, y);
            float cc  = heightAt(midX, midY);
            float pyc = heightAt(midX, prevY);
            float cbl = heightAt(x, nextY);
            float pxc = heightAt(prevX, midY);

            float clm = (ctl + cc + cbl + pxc) / 4.0f + randomFloatRange(-curheight, curheight);
            float ctm = (ctl + cc + ctr + pyc) / 4.0f + randomFloatRange(-curheight, curheight);

            setAt(midX, y, ctm);
            setAt(x, midY, clm);
        }
    }
}

float LIA::Terrain::randomFloatRange(float a, float b) {
    return a + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(b-a)));
}

void LIA::Terrain::makeIndices() {
    if (_drawAsPoints) {
        for (int i = 0; i < _model.data.vertices.size(); ++i) {
            _model._indices.push_back(i);
        }
    } else {
        for (int y = 0; y < _sizeY - 1; y++) {
            for (int x = 0; x < _sizeX - 1; x++) {
                unsigned int iBl = y * _sizeX + x;
                unsigned int iTl = (y + 1) * _sizeX + x;
                unsigned int iTR = (y + 1) * _sizeX + x + 1;
                unsigned int iBR = y * _sizeX + x + 1;

                _model._indices.push_back(iBl);
                _model._indices.push_back(iTl);
                _model._indices.push_back(iTR);

                _model._indices.push_back(iBl);
                _model._indices.push_back(iTR);
                _model._indices.push_back(iBR);
            }
        }
    }
    _model.hasIndices = true;
    _model.size = _model._indices.size();
}

bool LIA::Terrain::sendToGpu() {
    ShaderManager& shaderManager = LIA::Engine::getInstance().getShaderManager();
    ShaderProgram& shaderProgram = shaderManager.getShader(_shader);
 
    glUseProgram(shaderProgram.getProgram());
    VertexArrayObject vertexArrayObject;
    if (!vertexArrayObject.generate()) {
        return false;
    }
    if (!vertexArrayObject.bind()) {
        return false;
    }
    VertexBuffer& vertexBuffer = vertexArrayObject.getVertexBuffer();
    if (!vertexBuffer.generateAndBind()) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.setData(_model.data.vertices)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.enableAttributeArray(0)) {
        vertexArrayObject.destroy();
        return false;
    }
    if (!vertexBuffer.enableAttributePointer(0)) {
        vertexArrayObject.destroy();
        return false;
    }

    LIA_debug("Creating indices buffer");
    GLuint ib;
    glGenBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _model._indices.size() * sizeof(unsigned int), &_model._indices[0], GL_STATIC_DRAW);

    if (catchGlError()) {
        LIA_fatal("Failed to create index buffer for terrain");
        vertexArrayObject.destroy();
        return false;
    }

    _vao = vertexArrayObject.getId();
    _isInGpu = true;
    _model.isInGpu = true;
    LIA_debug(std::vformat("Vao {} created with size {}", std::make_format_args(_vao, _model.size)));
    return true;
}

bool LIA::Terrain::pass(Scene* scene) {
    Position position = emptyPosition();
    Scale scale = emptyScale();
    /**
      * string - identifier
      * Position - position
      * Rotation - rotation
      * Scale - scale
      * GLuint - vao
      * std::string - shader
      * bool - hasIndices
      * int - number of verteces/indices
      * materials
      * material ids
      * textures
      * bump textures
      * em textures
      * int - offset
      * bool - draw as points
      */
    if (!scene->add(
        "terrain",
        position,
        _rotation,
        scale,
        _vao,
        _shader,
        _model.hasIndices,
        _model.size,
        _model.data.materialLib,
        _model.data.materialName.size() < 1 ? "" : _model.data.materialName[0],
        /*
        _model._textures,
        _model._bump,
        _model._em,
        */
        0,
        _drawAsPoints
    )) {
        return false;
    }
    SceneObject* terrainObject = scene->get(0, 0, "terrain");
    terrainObject->_wireMode = _wireMode;

    return true;
}

void LIA::Terrain::setWire(bool wire) {
    _wireMode = wire;
    Scene* scene = Engine::getInstance().getTerrainRenderer();
    SceneObject* terrainObject = scene->get(0, 0, "terrain");
    terrainObject->_wireMode = _wireMode;
}
