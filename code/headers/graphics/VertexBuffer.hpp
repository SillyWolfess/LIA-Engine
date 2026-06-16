#ifndef LIA_VBUFFER_HPP
#define LIA_VBUFFER_HPP
#include "precompiled.hpp"

namespace LIA {
    class VertexBuffer {
        public:
            VertexBuffer();
            VertexBuffer(const VertexBuffer&) = delete;
            ~VertexBuffer();

            bool bind();
            bool unbind();
            bool generate();
            bool generateAndBind();

            bool enableAttributeArray(GLuint);
            bool enableAttributePointer(GLuint);
            bool enableAttributePointer(GLuint, int);
            bool setupArrayAndPointer(GLuint, int);

            bool setData(std::vector<glm::vec3>&);
            bool setData(std::vector<glm::vec2>&);

            void destroy();
            GLuint getId() { return _id; };
        private:
            GLuint _id;
            bool _ready = false;
    };
}
#endif