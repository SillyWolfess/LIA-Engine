#ifndef LIA_VAO_HPP
#define LIA_VAO_HPP
#include "precompiled.hpp"

#include "graphics/VertexBuffer.hpp"

namespace LIA {
    class VertexArrayObject {
        public:
            VertexArrayObject();
            VertexArrayObject(const VertexArrayObject&) = delete;
            ~VertexArrayObject();

            bool bind();
            bool unbind();
            bool generate();
            void destroy();
            GLuint getId() { return _id; };

            VertexBuffer& getVertexBuffer();
            VertexBuffer& getColorBuffer();
            VertexBuffer& getUvBuffer();
            VertexBuffer& getTBuffer() { return tBuffer; };
            VertexBuffer& getBBuffer() { return bBuffer; };
            VertexBuffer& getNormalBuffer() { return normalBuffer; };

        private:
            GLuint _id;
            bool _ready = false;
            VertexBuffer vertexBuffer;
            VertexBuffer colorBuffer;
            VertexBuffer uvBuffer;
            VertexBuffer tBuffer;
            VertexBuffer bBuffer;
            VertexBuffer normalBuffer;
    };
}
#endif