#ifndef LIA_WINDOW
#define LIA_WINDOW

#include <vector>
#include "guiObject.hpp"
#include "graphics/scene.hpp"
#include "graphics/font.hpp"
#include "data/Color.hpp"
#include "data/Padding.hpp"
#include "manager/eventManager.hpp"
#include "appWindow.hpp"

namespace LIA {
    struct Style {
        Color headerColor;
        Color headerHoverColor;
        Color headerGrabbedColor;

        Color buttonBgColor;
        Color hoverButtonBgColor;
        Color disbaledButtonColor;

        Color bgColor;
        Color hoverColor;
        Padding padding;

        Scale checkBoxScale;
        Color checkBoxCheckedColor;
        Color checkBoxUnCheckedColor;
        Color checkBoxHoverColor;

        int fontSize;
    };
    enum class ALIGN {NONE, RIGHT, CENTER, BOTTOM};
    class Window {
        private:
            ALIGN resolve(std::string);

            std::vector<GuiObject> _children;
            std::map<std::string, std::vector<GuiObject>> _childrenMap;
            
            Position _position;
            Position _mouseLastHoverPos;
            
            Scale _scale;
            Scale _lastAppScale;
            Scale _buttonScale;
            Scale _checkboxScale;
                        
            Color _headerColor;
            
            Style _style;

            std::string _path;
            std::string _name;
            std::string _id;
            ALIGN _alignment;
            ALIGN _initAlign;
        
            float _headerSize;
            bool _isHeaderHover;
            bool _isGrabbed;
            bool _isHovered;
            bool _isMovable;
        //    int _fontSize;
            bool _visible;
            bool _hasHeader;
            bool _needToResize;
            void setDefaults(GuiObject& object);
                        
            Position combinePositions(Position&, Position&);
            Position combinePositions(Position&, Position&, Padding&);
            /**
             * Align window components according to the screen
             */
            void compute(AppWindow*);
            void compute(AppWindow*, bool);
            void computeScale();
            Position computeFontPosition(GuiObject&, std::string);
            Position computeFontPosition(GuiObject&, std::string, bool);
            void passChild(GuiObject&, Scene*, Font*);
            void logGuiObject(GuiObject&);
        public:
            Window();
            ~Window();

            void setPath(std::string path) { _path = path; };
            std::string getPath() { return _path; };

            void setStyle(struct Style style) { _style = style; };
            void setAlignment(std::string value) { _alignment = resolve(value); };

            void setName(std::string name) { _name = name; };
            const std::string getName() { return _name; };

            void setId(std::string id) { _id = id; };
            const std::string getId() { return _id; };

            void hide();
            void show(AppWindow*);
            const bool isVisible() { return _visible; };

            void setPosition(float x, float y, float z);
            void setPosition(float x, float y);
            void setPosition(Position&);

            void setPadding(Padding&);

            void setScale(float x, float y);
            void setScale(Scale&);
            void setButtonScale(Scale&);
            void setCheckboxScale(Scale&);
            void setCheckboxCheckedColor(Color&);
            void setCheckboxUnCheckedColor(Color&);
            void setCheckboxHoverColor(Color&);
            void setDisabledButtonColor(Color&);

            void setHeader(bool, float, Color&);
            void setHeader(bool, float);
            void setHoverColor(Color&);
            void setBgColor(Color&);
            void setButtonBgColor(Color&);
            void setButtonHoverColor(Color&);

            Color defaultFontColor();
            Color defaultBgColor();
            Color defaultDisabledBgColor();
            Scale defaultScale();
            Rotation defaultRotation();
            int defaultFontSize();
            void setFontSize(int);

            void setInitAlign(std::string);
            void setMovable(bool);

            /***
             * Construct position from 2D cordinates
             * @param {float} x
             * @param {flaot} y
             * @return Position
             */
            Position makePosition(float, float);
            /***
             * Construct scale from 2D cordinates
             * @param {float} x
             * @param {flaot} y
             * @return Scale
             */
            Scale makeScale(float, float);
            /**
             * @param {string} id
             * @param {string} name
             */
            void addList(std::string, std::string);
            /**
             * @param {string} id
             * @param {string} text
             * @param {Position} position
             * @param {Scale} scale
             * @param {Color} font color
             * @param {string} event action
             * @param {string} event arg0
            */
            void addButton(std::string, std::string, Position&, Scale&, Color&, std::string, std::string);
            /**
             * @param {string} id
             * @param {string} text
             * @param {Position} position
             * @param {Scale} scale
             * @param {Color} font color
             * @param {string} event action
             * @param {string} event arg0
             * @param {string} texture
            */
            void addButton(std::string, std::string, Position&, Scale&, Color&, std::string, std::string, std::string);
            /**
             * @param {string} id
             * @param {string} text
             * @param {Position} position
             * @param {string} event action
             * @param {string} event arg0
            */
            void addButton(std::string ,std::string, Position&, std::string, std::string);
            /**
             * @param {string} id
             * @param {string} text
             * @param {string} event action
             * @param {string} event arg0
            */
            void addButton(std::string ,std::string, std::string, std::string);
            /**
             * @param {string} id
             * @param {string} text
             * @param {string} event action
             * @param {string} event arg0
             * @param {string} texture
            */
            void addButton(std::string, std::string, std::string, std::string, std::string);
            /***
             * @param {string} id
             * @param {string} text
             * @param {float} x
             * @param {float} y
            */
            void addButton(std::string, std::string, float, float);
            /**
             * @param {string} id
             * @param {string} label
             * @param {string} value
             * @param {string} placeholder
             * @param {Position} position
             * @param {string} texture
            */
            void addField(std::string, std::string, std::string, std::string, Position&, std::string);
            /**
             * @param {string} id
             * @param {string} label
             * @param {string} value
             * @param {string} placeholder
             * @param {string} texture
            */
            void addField(std::string, std::string, std::string, std::string, std::string);
            /**
             * @param {string} id
             * @param {string} label
             * @param {string} value
             * @param {string} placeholder
             * @param {Position} position
            */
            void addField(std::string, std::string, std::string, std::string, Position&);
            /**
             * @param {string} id
             * @param {string} label
             * @param {string} value
             * @param {string} placeholder
            */
            void addField(std::string, std::string, std::string, std::string);
            /**
             * @param {string} id
             * @param {string} name/label
             * @param {bool} value
             * @param {string} event action
             */
            void addCheckBox(std::string, std::string, bool, std::string);
            /**
             * @param {string} id
             * @param {string} value
             */
            void addLabel(std::string, std::string);
            /**
             * @param {string} id
             * @param {list<string>} list of values
             * @param {int} type of children
             */
            void updateList(std::string, std::vector<std::string>, int);
            /**
             * @param {string} id
             */
            void updateGrid(std::string);
            /**
             * @param {string} id
             * @param {string} value
             */
            void updateField(std::string, std::string);
            /**
             * @param {string} id
             * @param {bool} value
             */
            void enableField(std::string, bool);
            /**
             * @param {string} grid id
             * @param {string} child id
             * @param {bool} value
             */
            void enableGridField(std::string, std::string, bool);
            bool hasField(std::string);
            bool hasButton(std::string);
            void addGrid(std::string id, std::string name, int minRows, int maxRows, int minColumns, int maxColumns);
            void passObjects(Scene*, Font*);
            bool mouseHover(Position&);
            bool mouseClick(Position&, EventManager*);
            bool mouseDown(Position&, EventManager*);
            void mouseLastPosition(Position&);
            bool grab(Position&);
            bool isGrabbed() { return _isGrabbed; }
            bool isHovered() { return _isHovered; }
            void removeHover();
            void resize(AppWindow*);
            void clear();
    };
}
#endif