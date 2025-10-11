#include "graphics/VertexArrayObject.hpp"
#include "graphics/glwrapper.hpp"

LIA::VertexArrayObject::VertexArrayObject() {
    LIA_DEBUG_CONSTRUCTOR("VertexArrayObject");
    LIA_trace("Creating VertexArrayObject");
};

LIA::VertexArrayObject::~VertexArrayObject() {
    LIA_DEBUG_DESTRUCTOR("VertexArrayObject");
}

bool LIA::VertexArrayObject::generate() {
    if (_ready) {
        LIA_warn("VertexArrayObject is already generated. Making new.");
    }
    glGenVertexArrays(1, &_id);
    if (catchGlError()) {
        LIA_fatal("Failed to generate VertexArrayObject");
        return false;
    }
    LIA_trace_f("VertexArrayObject {} generated", _id);
    _ready = true;
    return true;
}

bool LIA::VertexArrayObject::bind() {
    if (!_ready) {
        LIA_fatal("VertexArrayObject is not generate");
        return false;
    }
    glBindVertexArray(_id);
    if (catchGlError()) {
        LIA_fatal_f("Failed to bind VertexArrayObject {}", _id);
        return false;
    }
    return true;
}

bool LIA::VertexArrayObject::unbind() {
    if (!_ready) {
        LIA_fatal("VertexArrayObject is not generate");
        return false;
    }
    glBindVertexArray(0);
    if (catchGlError()) {
        LIA_fatal_f("Failed to unbind VertexArrayObject {}", _id);
        return false;
    }
    return true;
}

void LIA::VertexArrayObject::destroy() {
    if (!_ready) {
        return;
    }
    vertexBuffer.destroy();
    colorBuffer.destroy();
    uvBuffer.destroy();
    normalBuffer.destroy();
    tBuffer.destroy();
    bBuffer.destroy();
    int i = _id;
    glDeleteVertexArrays(1, &_id);
    LIA_trace_f("VertexArrayObject {} destroyed", i);
}

LIA::VertexBuffer& LIA::VertexArrayObject::getVertexBuffer() {
    return vertexBuffer;
}

LIA::VertexBuffer& LIA::VertexArrayObject::getColorBuffer() {
    return colorBuffer;
}

LIA::VertexBuffer& LIA::VertexArrayObject::getUvBuffer() {
    return uvBuffer;
}
