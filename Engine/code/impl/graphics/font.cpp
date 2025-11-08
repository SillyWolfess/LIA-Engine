#include "graphics/font.hpp"
#include "precompiled.hpp"

#include <format>
#include <glm/gtc/matrix_transform.hpp>

#include "loaders/shaderLoader.hpp"
#include "loaders/objLoader.hpp"

#include "graphics/glwrapper.hpp"
#include "manager/shaderManager.hpp"


/*
void LIA::Font::line(float& y, float yp)
{
	y = y + yp;
}
*/
int LIA::Font::getFont(char font)
{
	int n = font;
	int f = fontMap_[n].id_;
	if(f > maxFonts - 1 || f < 0) {
		return 0;
	}
	return f;
}

void LIA::Font::drawText(glm::mat4 VP, Position position, Color color, int size, std::string string_)
{
	char my_font='A';
	int count = 0;

	float rotationX = 180;
	float rotationY = 270;
	float moveX = 0;

    int fontPos = 0;

	glEnableVertexAttribArray(0);
	glUniform3f(vertexColorLocation_, color.r, color.g, color.b);
    if (catchGlError()) {
        LIA_error("Failed to send color to shader");
    }

	while (fontPos < string_.size())
	{
		my_font = string_.at(fontPos);
        fontPos++;
		if(my_font<'A' || my_font>'Z') {
			if (my_font>='a' && my_font <='z') {
				// ok
			} else if (my_font>='0' && my_font<='9') {
				// ok
			} else if (my_font == ',' || my_font=='/' || my_font=='-' || my_font=='.' || my_font=='(' || my_font==')' || my_font=='+' || my_font == '?' || my_font == ':') {
				// ok
			} else if (my_font==' ') {
				moveX = moveX + (settings_.spaceLenght * size);
				count++;
				continue;
			} else if (my_font=='\0') {
				break;
			} else {
				continue;
			}
		}
		int fontId = getFont(my_font);
		Position newPosition;
		copy(newPosition, position);
		newPosition.x = newPosition.x + moveX;
		Rotation rotation;
		rotation.x = rotationX * PiOver180;
		rotation.y = rotationY * PiOver180;
		rotation.z = 0.0;
		Scale scale;
		setAll(scale, size);
		drawFont(VP, fontId, newPosition, rotation, scale);
		count++;
		moveX  = moveX + (font_[fontId].xSize_ * size) + (settings_.spacing * size);
	}
	counter_ = count + counter_;
	glDisableVertexAttribArray(0);
}
void LIA::Font::drawFont(glm::mat4& VP, int fontId, Position position, Rotation rotation, Scale scale) {
		
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(position.x, position.y, position.z));
	M = glm::rotate(M, rotation.x, glm::vec3(1, 0, 0));
	M = glm::rotate(M, rotation.y, glm::vec3(0, 1, 0));
	M = glm::rotate(M, rotation.z, glm::vec3(0, 0, 1));
	M = glm::scale(M, glm::vec3(scale.x, scale.y, scale.z));
	
	glm::mat4 MVP = VP * M;
	glUniformMatrix4fv(MVPMatrixID_, 1, GL_FALSE, glm::value_ptr(MVP));  
    if (catchGlError()) {
        LIA_error("Failed to send matrix to shader");
		return;
    }
	glBindBuffer(GL_ARRAY_BUFFER, font_[fontId].vertexBuffer_);
	if (catchGlError()) {
		LIA_error(std::vformat("Failed to bind buffer '{}' for font '{}'", std::make_format_args(font_[fontId].vertexBuffer_, fontId)));
		return;
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	if (catchGlError()) {
		LIA_error("Failed to set atrib pointer");
		return;
	}
	glDrawArrays(GL_TRIANGLES, 0, font_[fontId].verticesSize_);
	if (catchGlError()) {
		LIA_error(std::vformat("Failed to draw font {}", std::make_format_args(fontId)));
	}

}
void LIA::Font::draw(Camera &camera, ShaderManager* shaderManager) {
	programID_ = _shaderManager->getProgramId("font");
    prepareForDraw();
    glm::mat4 Projection = camera.getProjection();
    glm::mat4 View = camera.getView();
    glm::mat4 VP = Projection * View;

	for (Data& data: _data) {
		drawText(VP, data.position, data.color, data.size, data.text);
	}
}

void LIA::Font::bindFont(char font, int id)
{
	int n = font;
	fontMap_[n].id_ = id;
}
void LIA::Font::bindFont(int font, int id)
{
	fontMap_[font].id_ = id;
}
bool LIA::Font::load()
{
	LIA_info("Font: setting Fonts START");
	int id;
	
	XmlLoader xmlLoader;
    XmlLoader::XmlData xmlData = xmlLoader.load("./data/settings/font.xml");

	for (auto [name, path] : xmlData.values) {
		if (name == "") {
		//	LIA_fatal(std::vformat("Node without name with path '{}'", std::make_format_args(path)));
			LIA_fatal("Node without name with path '{}'", path);
			return false;
		}
		if (path == "") {
			LIA_fatal(std::vformat("Node without path with name '{}'", std::make_format_args(name)));
			return false;
		}
		LIA_trace(std::vformat("Name = '{}'; Path = '{}'", std::make_format_args(name, path)));
        XmlLoader::XmlData xmlFont = xmlLoader.load(path);
		LIA_trace(std::vformat("Getting font values from xml {}", std::make_format_args(path)));
        std::map<std::string, std::string> fontData = xmlFont.values;
		LIA_trace(std::vformat("Getting node {}", std::make_format_args(name)));
		XmlLoader::XmlNode node = xmlLoader.getNode(xmlData, name);
		LIA_trace(std::vformat("Getting folder attribute from {}", std::make_format_args(name)));
		std::string folderPath = xmlLoader.getAttribute(node, "folder");
		LIA_TRY
		for (auto [fontName, fontPath]: fontData) {
			id = addFont(folderPath.c_str(), fontPath.c_str());
			XmlLoader::XmlNode fontNode = xmlLoader.getNode(xmlFont, fontName);
			int ascii = xmlLoader.getInt(fontNode, "ascii", -1);
			bindFont(ascii != -1 ? ascii : fontName[0], id);
		}
		LIA_CATCH_RETURN_FALSE	
	}

	for(int i = 0; i <  nFonts_; i++)
	{
		ModelData object;
		fontLoaded_[i] = loadFontFromFile(i, object);
		if(!fontLoaded_[i]) {
			LIA_error(std::vformat("Font: buffer creation failed {} ", std::make_format_args(i)));
			return false;
		}
		// verteces
		glGenBuffers(1, &font_[i].vertexBuffer_);	
		glBindBuffer(GL_ARRAY_BUFFER, font_[i].vertexBuffer_);
		glBufferData(GL_ARRAY_BUFFER, object.vertices.size() * sizeof(glm::vec3),  &object.vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		if (catchGlError()) {
			LIA_fatal("Failed to enable vertex attribute array");
			return false;
		}
		glBindBuffer(GL_ARRAY_BUFFER, font_[i].vertexBuffer_);
		if (catchGlError()) {
			LIA_fatal("Failed to bind vbo");
			return false;
		}
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		if (catchGlError()) {
			LIA_fatal("Failed to set pointer 0");
			return false;
		}
				
		font_[i].verticesSize_ = object.vertices.size();
		font_[i].xSize_ = object.max_z - object.min_z;
        LIA_debug(std::vformat("Buffer {} created for font {}", std::make_format_args(font_[i].vertexBuffer_, i)));
	}
	LIA_info("Font: setting Fonts END");
	return true;
}
bool LIA::Font::loadFontFromFile(int id, ModelData& object)
{    
	LIA_debug(std::vformat("Loading {} from folder '{}' and file '{}'", std::make_format_args(id, font_[id].folder_.c_str(), font_[id].path_.c_str())));
	ObjLoader::load(
		object,
		font_[id].folder_.c_str(),
		font_[id].path_.c_str()
	);
	return object._complete;
}
int LIA::Font::addFont(const char* dir, const char* path)
{
	if(nFonts_ + 1 > maxFonts - 1)
		return - 1;
	
	font_[nFonts_].path_	= std::string(path);
	font_[nFonts_].folder_	= std::string(dir);
	nFonts_++;
	return nFonts_ - 1;
}
LIA::Font::Font()
{
	nFonts_ = 0;
	counter_= 0;
}
bool LIA::Font::initialise(ShaderManager* shanderManager)
{	
	_shaderManager = shanderManager;
	resolution_.x_ = 1920.0f;
	resolution_.y_ = 1080.0f;
	 
	programID_ = shanderManager->getProgramId("font");
	glUseProgram(programID_);
	
    glGenVertexArrays(1, &VertexArrayID_);
	glBindVertexArray(VertexArrayID_);
    if (catchGlError()) {
        LIA_fatal("Failed to create or bind vao for font");
        return false;
    }

	if (!getUniform(MVPMatrixID_, programID_, "MVP")) {
		LIA_fatal("Failed to get MVP uniform from gpu");
        return false;
    }
    if (!getUniform(vertexColorLocation_, programID_, "newColor")) {
		LIA_fatal("Failed to get newColor uniform from gpu");
        return false;
    }

	if (!load()) {
		LIA_fatal("Failed to load fonts");
		return false;
	}
	
	recalculateScale();

	settings_.spacing = 0.2;
	settings_.spaceLenght = 0.5;
	settings_.tabMultiplier = 4;
	
	LIA_info("font initialisation END");
	return true;
}
void LIA::Font::recalculateScale() {
	float scale		= resolution_.y_ / 50;
	if(resolution_.x_ / 100 < scale)
		scale = resolution_.x_ / 100;
	defaultScale_ = scale;
}
void LIA::Font::resizeScreen(int x, int y)
{
	resolution_.x_ = x;
	resolution_.y_ = y;
	
	recalculateScale();
	
    LIA_debug(std::vformat("New screen settings for Font: {} x {} -> scale = {}",std::make_format_args(x, y, defaultScale_)));
}
void LIA::Font::prepareForDraw()
{
	glGetError();
	glUseProgram(programID_);
	glBindVertexArray(VertexArrayID_);
	catchGlError();
}
LIA::Font::~Font()
{
    LIA_trace("Destroying Fonts");
	for(int i = 0; i < nFonts_; i++)
	{
		if(!fontLoaded_[i])
			continue;
		glDeleteBuffers(1, &font_[i].vertexBuffer_);
	}
}
LIA::Color LIA::Font::getDefaultColor() {
	return whiteColor();
}
int LIA::Font::getDefaultSize()
{
	return defaultScale_;
}

void LIA::Font::clearText() {
	int size = _data.size();
	_data.clear();
	_data.reserve(size);
}
void LIA::Font::addText(std::string text, Position position) {
	addText(text, position, getDefaultColor(), getDefaultSize());
}
void LIA::Font::addText(std::string text, Position position, Color color) {
	addText(text, position, color, getDefaultSize());
}
void LIA::Font::addText(std::string text, Position position, int size) {
	addText(text, position, getDefaultColor(), size);
}
void LIA::Font::addText(std::string text, Position position, Color color, int size) {
	Data& data = _data.emplace_back();
	data.text = text;
	data.position = position;
	data.color = color;
	data.size = size;
}