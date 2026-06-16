#include "graphics/VertexBuffer.hpp"
#include "graphics/glwrapper.hpp"

LIA::VertexBuffer::VertexBuffer() {
    LIA_DEBUG_CONSTRUCTOR("VertexBuffer");
    LIA_trace("Creating VertexBuffer");
};

LIA::VertexBuffer::~VertexBuffer() {
    LIA_DEBUG_DESTRUCTOR("VertexBuffer");
    LIA_trace_f("VertexBuffer {} destroyed", _id);
}

bool LIA::VertexBuffer::generateAndBind() {
    if (!generate()) {
        return false;
    }
    if (!bind()) {
        return false;
    }
    return true;
}

bool LIA::VertexBuffer::generate() {
    if (_ready) {
        LIA_warn("VertexBuffer is already generated. Making new.");
    }
    glGenBuffers(1, &_id);
    if (catchGlError()) {
        LIA_fatal("Failed to generate VertexBuffer");
        return false;
    }
    _ready = true;
    return true;
}

bool LIA::VertexBuffer::bind() {
    if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    if (catchGlError()) {
        LIA_fatal_f("Failed to bind VertexBuffer {}", _id);
        return false;
    }
    return true;
}

bool LIA::VertexBuffer::unbind() {
    if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (catchGlError()) {
        LIA_fatal_f("Failed to unbind VertexBuffer {}", _id);
        return false;
    }
    return true;
}
bool LIA::VertexBuffer::setData(std::vector<glm::vec3>& data) {
     if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);
    if (catchGlError()) {
        LIA_fatal_f("Failed to set buffer data for ", _id);
        return false;
    }
    return true;
}
bool LIA::VertexBuffer::setData(std::vector<glm::vec2>& data) {
     if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec2), &data[0], GL_STATIC_DRAW);
    if (catchGlError()) {
        LIA_fatal_f("Failed to set buffer data for ", _id);
        return false;
    }
    return true;
}
bool LIA::VertexBuffer::setupArrayAndPointer(GLuint id, int size) {
    if (!enableAttributeArray(id)) {
        return false;
    }
    if (!enableAttributePointer(id, size)) {
        return false;
    }
    return true;
}
bool LIA::VertexBuffer::enableAttributeArray(GLuint id) {
    if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
    glEnableVertexAttribArray(id);
    if (catchGlError()) {
        LIA_fatal_f("Failed to enable attribute array ", id);
        return false;
    }
    return true;
}

bool LIA::VertexBuffer::enableAttributePointer(GLuint id) {
    if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, NULL);
     if (catchGlError()) {
        LIA_fatal_f("Failed to enable attribute pointer {}", id);
        return false;
    }
    return true;
}

bool LIA::VertexBuffer::enableAttributePointer(GLuint id, int size) {
    if (!_ready) {
        LIA_fatal("VertexBuffer is not generate");
        return false;
    }
    glVertexAttribPointer(id, size, GL_FLOAT, GL_FALSE, 0, NULL);
     if (catchGlError()) {
        LIA_fatal_f("Failed to enable attribute pointer {}", id);
        return false;
    }
    return true;
}

void LIA::VertexBuffer::destroy() {
    if (!_ready) {
        return;
    }
    int i = _id;
    glDeleteBuffers(1, &_id);
    LIA_trace_f("VertexBuffer {} destroyed", i);
}