#ifndef LIA_FONT
#define LIA_FONT

#include "precompiled.hpp"
#include <vector>

#include "data/ModelData.hpp"
#include "data/position.hpp"
#include "data/Color.hpp"
#include "camera.hpp"

namespace LIA {
    class ShaderManager;
    class Font {
        private:
            ShaderManager* _shaderManager;
            static const int maxFonts = 256;
            const float PiOver180 = 0.0174532925f;
            
            GLuint programID_;
            GLint MVPMatrixID_;
            
            GLuint VertexArrayID_;
            
            GLint vertexColorLocation_;
            
            struct S_Font
            {
                std::string path_;
                std::string folder_;
                
                GLuint vertexBuffer_;
                
                int verticesSize_;
                float xSize_;
            };
            struct S_Settings {
                float spacing;
                float spaceLenght;
                int tabMultiplier;

            } settings_;
            
            struct Matrix
            {
                glm::vec3 position_;
            };
                        
            struct Map
            {
                int id_;
            };
            
            int nFonts_;
            int counter_;
            
            float defaultScale_;
            struct Resolution
            {
                float x_;
                float y_;
            } resolution_;
                        
            struct Map fontMap_[maxFonts];
            struct Matrix matrix_;
            struct S_Font font_[maxFonts];
            bool fontLoaded_[maxFonts];

            void recalculateScale();

            void drawText(glm::mat4, Position, Color, int, std::string);
            void drawFont(glm::mat4&, int, Position, Rotation, Scale);

            bool load();
            bool loadFontFromFile(int, ModelData&);

            void bindFont(char, int);
            void bindFont(int, int);

            int addFont(const char*, const char*);
            int getFont(char);
            
            void prepareForDraw();

            struct Data {
                std::string text;
                Position position;
                Color color;
                int size;
            };
            std::vector<Data> _data;
        public:
            Font();
            ~Font();

            bool initialise(ShaderManager*);
            /** resize drawing area
            *	@param {int} width
            *	@param {int} height
            */
            void resizeScreen(int, int);
            int getDefaultSize();
            Color getDefaultColor();
            
            void addText(std::string, Position);
            void addText(std::string, Position, int);
            void addText(std::string, Position, Color);
            void addText(std::string, Position, Color, int);
            void clearText();

            void draw(Camera&, ShaderManager*);
    };
}
#endif