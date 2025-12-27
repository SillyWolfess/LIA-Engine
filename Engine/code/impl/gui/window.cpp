#include "gui/window.hpp"
#include "logs.hpp"
#include "Engine.hpp"

LIA::Window::Window() : _isMovable{false}, _visible{false}, _isGrabbed{false}, _needToResize{false}
{
    LIA_trace("Window constructor");
}

LIA::Window::~Window() {
    LIA_trace("Window destructor");
}

void LIA::Window::hide() {
    _visible = false;
    _isHeaderHover = false;
    _isGrabbed = false;
}

void LIA::Window::show(AppWindow* appWindow) {
    _visible = true;
    _isHeaderHover = false;
    _isGrabbed = false;
    _lastAppScale = appWindow->getWindowScale();
    compute(appWindow, true);
}

void LIA::Window::setPosition(float x, float y, float z) {
    _position.x = x;
    _position.y = y;
    _position.z = z;
}

void LIA::Window::setPosition(Position& position) {
    copy(_position, position);
}

void LIA::Window::setPadding(Padding& padding) {
    copyPadding(_style.padding, padding);
}

void LIA::Window::setPosition(float x, float y) {
    _position.x = x;
    _position.y = y;
}

void LIA::Window::setScale(float x, float y) {
    _scale.x = x;
    _scale.y = y;
    _scale.z = 1;
}

void LIA::Window::setScale(Scale& scale) {
    copy(_scale, scale);
}

void LIA::Window::setButtonScale(Scale& scale) {
    copy(_buttonScale, scale);
}

void LIA::Window::setCheckboxScale(Scale& scale) {
    copy(_style.checkBoxScale, scale);
}

void LIA::Window::setCheckboxCheckedColor(Color& color) {
    copyColor(_style.checkBoxCheckedColor, color);
}

void LIA::Window::setCheckboxUnCheckedColor(Color& color) {
    copyColor(_style.checkBoxUnCheckedColor, color);
}

void LIA::Window::setDisabledButtonColor(Color& color) {
    copyColor(_style.disbaledButtonColor, color);
}

void LIA::Window::setCheckboxHoverColor(Color& color) {
    copyColor(_style.checkBoxHoverColor, color);
}

void LIA::Window::setHoverColor(Color& color) {
    copyColor(_style.hoverColor, color);
}

void LIA::Window::setButtonHoverColor(Color& color) {
    copyColor(_style.hoverButtonBgColor, color);
}

void LIA::Window::setBgColor(Color& color) {
    copyColor(_style.bgColor, color);
}

void LIA::Window::setButtonBgColor(Color& color) {
    copyColor(_style.buttonBgColor, color);
}

void LIA::Window::addButton(std::string id, std::string text, Position& position, Scale& scale, Color& fontColor, std::string eventAction, std::string eventArg0) {
    GuiObject button;
    button._id = id;
    button._type = GuiObjectType::BUTTON;
    button._labelAlignment = GUI_LABEL_POSITION::PREFIX;
    button._value = text;
    button._position = position;
    button._scale = scale;
    button._fontColor = fontColor;
    button._action = eventAction;
    button._arg0 = eventArg0;

    button._fontSize = defaultFontSize();
    button._disabledBgColor = defaultDisabledBgColor();
    button._bgColor = defaultBgColor();
    
    button._isHovered = false;
    button._enabled = true;
    logGuiObject(button);
    _children.push_back(button);
    _needToResize = true;
}

void LIA::Window::logGuiObject(GuiObject& object) {
    LIA_debug_f(
        "type = {}, value = {}, enabled = {}",
        objectTypeToString(object._type),
        object._value,
        object._enabled
    );
}

void LIA::Window::addButton(std::string id, std::string text, float x, float y) {
    Position position = makePosition(x, y);
    addButton(id, text, position, "", "");
}

void LIA::Window::addButton(std::string id, std::string text, std::string eventAction, std::string eventArg0) {
    Scale scale = defaultScale();
    Color fontColor = defaultFontColor();
    Position empty = emptyPosition();
    addButton(id, text, empty, scale, fontColor, eventAction, eventArg0);
}
void LIA::Window::addButton(std::string id, std::string text, Position& position, std::string eventAction, std::string eventArg0) {
    Scale scale = defaultScale();
    Color fontColor = defaultFontColor();
    addButton(id, text, position, scale, fontColor, eventAction, eventArg0);
}

void LIA::Window::addField(std::string id, std::string label, std::string value, std::string placeholder) {
    Position empty = emptyPosition();
    addField(id, label, value, placeholder, empty);
}

bool LIA::Window::hasField(std::string id) {
    for (GuiObject& child: _children) {
        if (child._id.compare(id) == 0) {
            if (child._type == GuiObjectType::FIELD) {
                return true;
            }
        }
    }
    return false;
}

bool LIA::Window::hasButton(std::string id) {
    for (GuiObject& child: _children) {
        if (child._id.compare(id) == 0) {
            if (child._type == GuiObjectType::BUTTON) {
                return true;
            }
        }
    }
    return false;
}

void LIA::Window::addCheckBox(std::string id, std::string label, bool isChecked, std::string eventAction) {
    GuiObject checkbox;
    setDefaults(checkbox);
    
    checkbox._id = id;
    checkbox._type = GuiObjectType::CHECKBOX;
    checkbox._labelAlignment = GUI_LABEL_POSITION::SUFFIX;
    checkbox._label = label;
    checkbox._valueB = isChecked;
    checkbox._action = eventAction;

    checkbox._scale = _style.checkBoxScale;
    _children.push_back(checkbox);
}

void LIA::Window::setDefaults(GuiObject& object) {
    object._scale = defaultScale();
    object._fontSize = defaultFontSize();
    object._fontColor = defaultFontColor();
    object._bgColor = defaultBgColor();

    object._isHovered = false;
    object._enabled = true;
}

void LIA::Window::addLabel(std::string id, std::string value) {
    GuiObject label;
    setDefaults(label);

    label._id = id;
    label._type = GuiObjectType::LABEL;
    label._labelAlignment = GUI_LABEL_POSITION::PREFIX;
    label._value = value;
    _children.push_back(label);
}

void LIA::Window::addGrid(std::string id, std::string name, int minRows, int maxRows, int minColumns, int maxColumns) {
    for (int y = 0; y < maxColumns; y++) {
        for (int x = 0; x < maxRows; x++) {
            int n = x + (y * maxRows);
            GuiObject grid;
            grid._id = std::vformat("{}[{}]", std::make_format_args(id, n));
            grid._type = GuiObjectType::GRID;
            grid._labelAlignment = GUI_LABEL_POSITION::PREFIX;
            grid._value = "";

            setDefaults(grid);
            _children.push_back(grid);
        }
    }
}

void LIA::Window::addList(std::string id, std::string name) {
    GuiObject data;
    setDefaults(data);
    data._id = id;
    data._label = name;
    data._type = GuiObjectType::LIST;
    _children.push_back(data);
    std::vector<GuiObject> tmp;
    _childrenMap.emplace(std::pair<std::string, std::vector<GuiObject>>(id, tmp));
}

void LIA::Window::addField(std::string id, std::string label, std::string value, std::string placeholder, Position& position) {
    GuiObject field;
    setDefaults(field);

    field._id = id;
    field._type = GuiObjectType::FIELD;
    field._labelAlignment = GUI_LABEL_POSITION::PREFIX;
    field._label = label;
    field._value = value;
    field._position = position;
    field._placeholder = placeholder;

    _children.push_back(field);
}

void LIA::Window::enableField(std::string id, bool value) {
    for (GuiObject& child: _children) {
        if (child._id.compare(id) == 0) {
            if (child._type == GuiObjectType::BUTTON) {
                child._enabled = value;
            }
            return;
        }
    }
}

void LIA::Window::updateField(std::string id, std::string value) {
    for (GuiObject& child: _children) {
        if (child._id.compare(id) == 0) {
            if (child._type == GuiObjectType::FIELD) {
                child._value = value;
            }
            else if (child._type == GuiObjectType::CHECKBOX) {
                child._valueB = value.compare("true") == 0;
            }
            else if (child._type == GuiObjectType::BUTTON) {
                child._value = value;
            }
            return;
        }
    }
}

void LIA::Window::updateList(std::string id, std::vector<std::string> value, int subtype) {
    for (GuiObject& child: _children) {
        if (child._id.compare(id) == 0) {
            if (child._type == GuiObjectType::LIST) {
                std::vector<GuiObject>& data = _childrenMap[child._id];
                data.clear();
                data.reserve(value.size());
                int indx = 0;
                for (std::string &v : value) {
                    GuiObject& lgu = data.emplace_back();
                    setDefaults(lgu);

                    if (subtype == GuiObjectType::BUTTON) {
                        lgu._type = GuiObjectType::BUTTON;
                    } else {
                        lgu._type = GuiObjectType::FIELD;
                    }
                    lgu._id = std::vformat("{}[{}]", std::make_format_args(child._id, indx));
                    lgu._value = v;
                    indx++;
                }
                _needToResize = true;
            }
            return;
        }
    }
}

LIA::Position LIA::Window::makePosition(float x, float y) {
    Position pos = emptyPosition();
    pos.x = x;
    pos.y = y;
    return pos;
}

LIA::Scale LIA::Window::makeScale(float x, float y) {
    Scale scale = emptyScale();
    scale.x = x;
    scale.y = y;
    return scale;
}


LIA::Scale LIA::Window::defaultScale() {
   return _buttonScale;
}

LIA::Color LIA::Window::defaultFontColor() {
    return whiteColor();
}

LIA::Color LIA::Window::defaultBgColor() {
   return _style.buttonBgColor;
}

LIA::Color LIA::Window::defaultDisabledBgColor() {
    return _style.disbaledButtonColor;
}

LIA::Rotation LIA::Window::defaultRotation() {
    return emptyRotation();
}

int LIA::Window::defaultFontSize() { return _style.fontSize; }

void LIA::Window::setFontSize(int fontSize) {
    _style.fontSize = fontSize;
}
void LIA::Window::resize(AppWindow* appWindow) {
    if (!_visible) {
        return;
    }
    compute(appWindow);
}

void LIA::Window::clear() {
    _children.clear();
    _childrenMap.clear();
}

void LIA::Window::compute(AppWindow* appWindow) {
    compute(appWindow, false);
}

void LIA::Window::computeScale() {
    float yShift = _style.padding.top;
    float xMax = 0.0f;
    if (_hasHeader) {
        yShift = yShift + _headerSize;
    }
    int gridX = 0;
    for (GuiObject& button : _children) {
        if (button._type == GuiObjectType::GRID) {
            gridX++;
        }
        if (button._type == GuiObjectType::GRID) {
        //    yShift = yShift + button._scale.y + _style.padding.bottom;
        } else if (button._type == GuiObjectType::LIST) {
            for (GuiObject& ch: _childrenMap[button._id]) {
                yShift = yShift + ch._scale.y + _style.padding.bottom;
            }
        } else {
            yShift = yShift + button._scale.y + _style.padding.bottom;
        }
        if (_style.padding.left + button._scale.x + _style.padding.right > xMax) {
            xMax = _style.padding.left + button._scale.x + _style.padding.right;
        }
    }
    if (_scale.y < yShift) {
        _scale.y = yShift;
    }
    if (_scale.x < xMax) {
        _scale.x = xMax;
    }
}

void LIA::Window::compute(AppWindow* appWindow, bool initShow) {
    Scale appScale = appWindow->getWindowScale();
    float zOffset = 0.0f;
    if (appScale.x == 0 || appScale.y == 0) {
        return;
    }
    _needToResize = false;
    computeScale();
    if (initShow) {
        if (_initAlign.compare("center") == 0) {
            _position.x = appScale.x * 0.5f - (_scale.x * 0.5f);
            _position.y = appScale.y * 0.5f - (_scale.y * 0.5f);
        }
    }
    _position.x = _position.x * (appScale.x / _lastAppScale.x);
    _position.y = _position.y * (appScale.y / _lastAppScale.y);
    float yShift = _style.padding.top;
//    float xMax = 0.0f;
    if (_hasHeader) {
        yShift = yShift + _headerSize;
    }
    if (_alignment.compare("bottom") == 0) {
        _position.y = appScale.y - _scale.y - yShift;
        LIA_debug_f("bottom[{}] => {:.2f}", _name, _position.y);
    } else {
        LIA_trace_f("aligment[{}] = {}", _name , _alignment);
    }
    int gridX = 0;
    for (GuiObject& button : _children) {
        button._position.x = _position.x + _style.padding.left;
        if (button._type == GuiObjectType::GRID) {
            button._position.x = button._position.x + (gridX * button._scale.x);
            gridX++;
        }
        button._position.y = _position.y + yShift;
        button._position.z = _position.z + zOffset;
        if (button._type == GuiObjectType::GRID) {
        //    yShift = yShift + button._scale.y + _style.padding.bottom;
        } else if (button._type == GuiObjectType::LIST) {
            button._position.y = _position.y;
            for (GuiObject& ch: _childrenMap[button._id]) {
                ch._position.x = button._position.x;
                ch._position.y = button._position.y + yShift;
                ch._position.z = button._position.z;
                LIA_trace_f("yShift = {:.2f} + {:.2f} + {:.2f} = {:.2f}", yShift, ch._scale.y, _style.padding.bottom, (yShift + ch._scale.y + _style.padding.bottom));
                yShift = yShift + ch._scale.y + _style.padding.bottom;
                LIA_trace_f("button[{}].position = {:.2f} x {:.2f} x {:.2f}", ch._id, ch._position.x, ch._position.y, ch._position.z);
            }
        } else {
            yShift = yShift + button._scale.y + _style.padding.bottom;
        }

        /*
        if (_style.padding.left + button._scale.x + _style.padding.right > xMax) {
            xMax = _style.padding.left + button._scale.x + _style.padding.right;
        }
        */
        LIA_trace_f("button[{}].position = {:.2f} x {:.2f} x {:.2f}", button._id, button._position.x, button._position.y, button._position.z);
    }
    /*
    if (_scale.y < yShift) {
        _scale.y = yShift;
    }
    if (_scale.x < xMax) {
        _scale.x = xMax;
    }
    */
    _lastAppScale = appScale;
}

void LIA::Window::passChild(GuiObject& child, Scene* scene, Font* font) {
    Rotation rotation = emptyRotation();
    if (child._type == GuiObjectType::FIELD) {
        std::string fieldText = (child._label.compare("") != 0 ? child._label + ": " : "") + (child._value.compare("") == 0 ? child._placeholder : child._value);
//        font->addText(fieldText, computeFontPosition(child, fieldText), child._fontColor, child._fontSize);
        scene->addText(font, fieldText, computeFontPosition(child, fieldText), child._fontColor, child._fontSize);
    } else if (child._type == GuiObjectType::BUTTON) {
    //    font->addText(child._value, computeFontPosition(child, child._value), child._isHovered ? _style.hoverColor : child._fontColor, child._fontSize);
        Color bgColor = child._isHovered ? _style.hoverButtonBgColor : child._bgColor;
        if (!child._enabled) {
            copyColor(bgColor, _style.disbaledButtonColor);
        }
        scene->addSquare(child._id, child._position, rotation, child._scale, bgColor);
        scene->addText(font, child._value, computeFontPosition(child, child._value), child._isHovered ? _style.hoverColor : child._fontColor, child._fontSize);
    } else if (child._type == GuiObjectType::CHECKBOX) {
        Color fontColor = child._valueB ? _style.checkBoxCheckedColor : _style.checkBoxUnCheckedColor;
        //font->addText(child._label, computeFontPosition(child, child._label, true), fontColor, child._fontSize);
        Color bgColor = child._valueB ? _style.checkBoxCheckedColor : _style.checkBoxUnCheckedColor;
        if (!child._enabled) {
            copyColor(bgColor, _style.disbaledButtonColor);
        }
        scene->addSquare(child._id, child._position, rotation, child._scale, child._isHovered ? _style.checkBoxHoverColor : bgColor);
        scene->addText(font, child._label, computeFontPosition(child, child._label, true), fontColor, child._fontSize);
    } else if (child._type == GuiObjectType::LABEL) {
    //    font->addText(child._value, computeFontPosition(child, child._value), child._fontColor, child._fontSize);
        scene->addText(font, child._value, computeFontPosition(child, child._value), child._fontColor, child._fontSize);
    } else {
        LIA_error(std::vformat("unknown gui object type of {}", std::make_format_args(static_cast<int>(child._type))));
    }
}

void LIA::Window::passObjects(Scene* scene, Font* font) {
    if (!_visible) {
        return;
    }
    if (_needToResize) {
        compute(&(Engine::getInstance().getAppWindow()));
    }
    Rotation rotation = emptyRotation();
    scene->increaseLayerId();
    scene->addSquare(_id, _position, rotation, _scale, _style.bgColor);

    for (GuiObject &child: _children) {
        if (child._type == GuiObjectType::LIST) {
            std::vector<GuiObject>& lChildren = _childrenMap[child._id];
            for (GuiObject& lchild: lChildren) {
                passChild(lchild, scene, font);
            }
        } else if (child._type == GuiObjectType::GRID) { 
            scene->addSquare(child._id, child._position, rotation, child._scale, child._isHovered ? _style.hoverButtonBgColor : child._bgColor);   
        } else {
           passChild(child, scene, font);
        }
    }

    if (_hasHeader) {
        GuiObject oHeader;
        oHeader._position.x = _position.x;
        oHeader._position.y = _position.y;
        oHeader._position.z = _position.z;// + 0.1f;
        oHeader._fontSize = defaultFontSize();
        oHeader._scale.x = _scale.x;
        oHeader._scale.y = _headerSize;
        oHeader._scale.z = 1.0f;
        scene->addSquare("header", oHeader._position, rotation, oHeader._scale, _isGrabbed ? (_style.headerGrabbedColor) : (_isHeaderHover ? _style.headerHoverColor : _headerColor));
   //     font->addText(_name, computeFontPosition(oHeader, _name), oHeader._fontSize);
        scene->addText(font, _name, computeFontPosition(oHeader, _name), oHeader._fontSize);
    }
}

bool LIA::Window::grab(Position& pos) {
    if (!_visible || !_isMovable || !_isHeaderHover) {
        return false;
    }
    _isGrabbed = true;
    return _isGrabbed;
}

bool LIA::Window::mouseDown(Position& pos, EventManager* eventManager) {
    if (!_visible || !_isGrabbed) {
        return false;
    }
    Position diff;
    diff.x = _mouseLastHoverPos.x - _position.x;
    diff.y = _mouseLastHoverPos.y - _position.y;
    _position.x = pos.x - diff.x;
    _position.y = pos.y - diff.y;
    compute(&(Engine::getInstance().getAppWindow()));
    return true;
}

bool LIA::Window::mouseClick(Position& pos, EventManager* eventManager) {
    if (!_visible) {
        return false;
    }
    _isGrabbed = false;
    if (!mouseHover(pos)) {
        return false;
    }
    for (GuiObject &child: _children) {
        if (child._isHovered) {
            if (!child._enabled) {
                return true;
            }
            if (child._type == GuiObjectType::BUTTON) {
                LIA_debug(std::vformat("Mouse click {}", std::make_format_args(child._id)));
                ButtonEvent buttonEvent(
                    std::vformat("{}_{}", std::make_format_args(_id, child._id)),
                    child._action,
                    child._arg0,
                    _id
                );
                eventManager->handleEvent(buttonEvent);
                return true;
            }
            else if (child._type == GuiObjectType::CHECKBOX) {
                LIA_debug(std::vformat("Mouse click {}", std::make_format_args(child._id)));
                CheckBoxEvent checboxEvent(
                    _id,
                    child._action
                );
                eventManager->handleEvent(checboxEvent);
                return true;
            }
        }
    }
    return false;
}

void LIA::Window::mouseLastPosition(Position& pos) {
    _mouseLastHoverPos = pos;
}

bool LIA::Window::mouseHover(Position& pos) {
    if (!_visible) {
        return false;
    }
    bool found = false;
    _isHeaderHover = false;
    for (GuiObject& child : _children) {
        if (child._type == GuiObjectType::BUTTON || child._type == GuiObjectType::CHECKBOX) {
            if (isInRange2D(pos, child._position, child._scale)) {
                child._isHovered = true;
                found = true;
            } else {
                child._isHovered = false;
            }
        }
    }
    if (_isMovable && !found && _hasHeader) {
        GuiObject oHeader;
        oHeader._scale.x = _scale.x;
        oHeader._scale.y = _headerSize;
        oHeader._scale.z = 1.0f;
        if (isInRange2D(pos, _position, oHeader._scale)) {
            _isHeaderHover = true;
            return true;
        }
    }
    return found;
}
LIA::Position LIA::Window::combinePositions(Position& pos1, Position& pos2) {
    Padding empty = emptyPadding();
    return combinePositions(pos1, pos2, empty);
}

LIA::Position LIA::Window::combinePositions(Position& pos1, Position& pos2, Padding& padding) {
    Position position;
    position.x = pos1.x + pos2.x + padding.left;
    position.y = pos1.y + pos2.y + padding.top;
    position.z = pos1.z + pos2.z + 0.1f;
    return position;
}
LIA::Position LIA::Window::computeFontPosition(GuiObject& guiObject, std::string value) {
    return computeFontPosition(guiObject, value, false);
}
LIA::Position LIA::Window::computeFontPosition(GuiObject& guiObject, std::string value, bool align) {
    Position position;
    Scale& scale = guiObject._scale;
    copy(position, guiObject._position);

    float emptySpace = scale.y - guiObject._fontSize;
    position.y = position.y + scale.y;
    if (emptySpace > 0.0f) {
        position.y = position.y - (emptySpace * 0.5f);
    }
    position.z = position.z + 0.5f;

    if (align) {
        if (guiObject._labelAlignment == GUI_LABEL_POSITION::SUFFIX) {
            position.x = position.x + scale.x + (scale.x * 0.1f);
        }
    }
    /*
    float fontLength = guiObject._fontSize * value.length();
    if (fontLength < scale.x) {
        float emptyX = scale.x - fontLength;
        position.x = position.x + (emptyX * 0.5f);
    }
    */
    return position;
}

void LIA::Window::setHeader(bool visible, float size, Color& color) {
    _hasHeader = visible;
    _headerSize = size;
    _headerColor = color;
}

void LIA::Window::setHeader(bool visible, float size) {
    _hasHeader = visible;
    _headerSize = size;
    _headerColor = _style.headerColor;
}

void LIA::Window::setInitAlign(std::string initAlign) {
    _initAlign = initAlign;
}
void LIA::Window::setMovable(bool movable) {
    _isMovable = movable;
}