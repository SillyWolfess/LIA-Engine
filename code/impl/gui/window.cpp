#include "gui/window.hpp"
#include "tools.hpp"
#include "logs.hpp"
#include "Engine.hpp"

LIA::Window::Window() : _isMovable{false}, _visible{false}, _isGrabbed{false}, _needToResize{false}, _isHovered{false}
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
    _isHovered = false;
}

void LIA::Window::show(AppWindow* appWindow) {
    _visible = true;
    _isHeaderHover = false;
    _isGrabbed = false;
    _isHovered = false;
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
    addButton(id, text, position, scale, fontColor, eventAction, eventArg0, "");
}

void LIA::Window::addButton(std::string id, std::string text, Position& position, Scale& scale, Color& fontColor, std::string eventAction, std::string eventArg0, std::string texture) {
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
    button._texture = texture;

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


void LIA::Window::addButton(std::string id, std::string text, std::string eventAction, std::string eventArg0, std::string texture) {
    Scale scale = defaultScale();
    Color fontColor = defaultFontColor();
    Position empty = emptyPosition();
    addButton(id, text, empty, scale, fontColor, eventAction, eventArg0, texture);
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

void LIA::Window::addField(std::string id, std::string label, std::string value, std::string placeholder, std::string texture) {
    Position empty = emptyPosition();
    addField(id, label, value, placeholder, empty, texture);
}

void LIA::Window::addField(std::string id, std::string label, std::string value, std::string placeholder, Position& position) {
    addField(id, label, value, placeholder, position, "");
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
    object._texture = "";

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

void LIA::Window::enableGridField(std::string id, std::string childId, bool value) {
    std::vector<GuiObject>& children = _childrenMap[id];
    for (GuiObject &child: children) {
        if (equals(child._id, childId)) {
            if (child._type == GuiObjectType::BUTTON) {
                child._enabled = value;
            }
            break;
        }
    }
}

void LIA::Window::addGrid(std::string id, std::string name, int minRows, int maxRows, int minColumns, int maxColumns) {
    GuiObject data;
    setDefaults(data);
    data._id = id;
    data._label = name;
    data._type = GuiObjectType::GRID;
    _children.push_back(data);
    std::vector<GuiObject> tmp;
    _childrenMap.emplace(std::pair<std::string, std::vector<GuiObject>>(data._id, tmp));

    std::vector<GuiObject>& children = _childrenMap[data._id];
    for (int y = 0; y < maxRows; y++) {
        for (int x = 0; x < maxColumns; x++) {
            GuiObject& ggo = children.emplace_back();
            setDefaults(ggo);
            ggo._enabled = false;
            ggo._type = GuiObjectType::BUTTON;
            ggo._id = std::vformat("{}[{}][{}]", std::make_format_args(data._id, x, y));
            ggo._grid.x = x;
            ggo._grid.y = y;
            ggo._scale.x = ggo._scale.x * 2.0f;
            ggo._scale.y = ggo._scale.x;
            ggo._value = "";
        }
    }
    
    updateGrid(data._id);
}

void LIA::Window::updateGrid(std::string id) {
    for (GuiObject& child : _children) {
        if (child._id.compare(id) != 0 || child._type != GuiObjectType::GRID) {
            continue;
        }
        std::vector<GuiObject>& data = _childrenMap[child._id];
        int indx = 0;
        for (GuiObject& go : data) {
            go._value = std::vformat("{}=[{},{}]", std::make_format_args(indx, go._grid.x, go._grid.y));
            indx++;
        }
        _needToResize = true;
        return;
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

void LIA::Window::addField(std::string id, std::string label, std::string value, std::string placeholder, Position& position, std::string texture) {
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
        if (child._id.compare(id) != 0 || child._type != GuiObjectType::LIST) {
            continue;
        }
        
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
        return;
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
            int lastY = 0;
            float lastScaleY = 0.0f;
            for (GuiObject& ch: _childrenMap[button._id]) {
                float xSize = _style.padding.left + (ch._grid.x * (ch._scale.x + _style.padding.left)) + ch._scale.x + _style.padding.right;
                if (xSize > xMax) {
                    xMax = xSize;
                }
                if (ch._grid.y > lastY) {
                    yShift = yShift + ch._scale.y + _style.padding.bottom;
                }
                lastY = ch._grid.y;
                lastScaleY = ch._scale.y + _style.padding.bottom;
            }
            yShift = yShift + lastScaleY;
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
        if (_initAlign == ALIGN::CENTER) {
            _position.x = appScale.x * 0.5f - (_scale.x * 0.5f);
            _position.y = appScale.y * 0.5f - (_scale.y * 0.5f);
        }
        else if (_initAlign == ALIGN::RIGHT) {
            _position.x = appScale.x - _scale.x;
            _position.y = _position.y * (appScale.y / _lastAppScale.y);
        }
    }
    if (_alignment == ALIGN::RIGHT) {
        _position.x = appScale.x - _scale.x;
        _position.y = _position.y * (appScale.y / _lastAppScale.y);
    }
    else {
        _position.x = _position.x * (appScale.x / _lastAppScale.x);
        _position.y = _position.y * (appScale.y / _lastAppScale.y);
    }
    float yShift = _style.padding.top;
    if (_hasHeader) {
        yShift = yShift + _headerSize;
    }
    if (_alignment == ALIGN::BOTTOM) {
        _position.y = appScale.y - _scale.y - yShift;
        LIA_debug_f("bottom[{}] => {:.2f}", _name, _position.y);
    }
    else if (initShow && _initAlign == ALIGN::BOTTOM) {
        _position.y = appScale.y - _scale.y - yShift;
    } 
    int gridX = 0;
    for (GuiObject& button : _children) {
        button._position.x = _position.x + _style.padding.left;
        button._position.y = _position.y + yShift;
        button._position.z = _position.z + zOffset;
        if (button._type == GuiObjectType::GRID) {
            button._position.y = _position.y;
            int lastY = 0;
            for (GuiObject& ch: _childrenMap[button._id]) {
                if (ch._grid.y > lastY) {
                    yShift = yShift + ch._scale.y + _style.padding.bottom;
                }
                ch._position.x = button._position.x + (ch._grid.x * (ch._scale.x + _style.padding.left));
                ch._position.y = button._position.y + yShift;
                lastY = ch._grid.y;
            }
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

        LIA_trace_f("button[{}].position = {:.2f} x {:.2f} x {:.2f}", button._id, button._position.x, button._position.y, button._position.z);
    }
    _lastAppScale = appScale;
}

void LIA::Window::passChild(GuiObject& child, Scene* scene, Font* font) {
    Rotation rotation = emptyRotation();
    if (child._type == GuiObjectType::FIELD) {
        std::string fieldText = (child._label.compare("") != 0 ? child._label + ": " : "") + (child._value.compare("") == 0 ? child._placeholder : child._value);
        scene->addText(font, fieldText, computeFontPosition(child, fieldText), child._fontColor, child._fontSize);
    } else if (child._type == GuiObjectType::BUTTON) {
        bool hasTexture = child._texture.compare("") != 0;
        Color bgColor = child._isHovered ? _style.hoverButtonBgColor : (hasTexture ? whiteColor() : child._bgColor);
        if (!child._enabled) {
            copyColor(bgColor, _style.disbaledButtonColor);
        }
        if (!hasTexture) {
            scene->addSquare(child._id, child._position, rotation, child._scale, bgColor);
        } else {
            scene->addSprite(child._id, child._position, rotation, child._scale, bgColor, child._texture);
        }
        
        scene->addText(font, child._value, computeFontPosition(child, child._value), child._isHovered ? _style.hoverColor : child._fontColor, child._fontSize);
    } else if (child._type == GuiObjectType::CHECKBOX) {
        Color fontColor = child._valueB ? _style.checkBoxCheckedColor : _style.checkBoxUnCheckedColor;
        Color bgColor = child._valueB ? _style.checkBoxCheckedColor : _style.checkBoxUnCheckedColor;
        if (!child._enabled) {
            copyColor(bgColor, _style.disbaledButtonColor);
        }
        scene->addSquare(child._id, child._position, rotation, child._scale, child._isHovered ? _style.checkBoxHoverColor : bgColor);
        scene->addText(font, child._label, computeFontPosition(child, child._label, true), fontColor, child._fontSize);
    } else if (child._type == GuiObjectType::LABEL) {
        scene->addText(font, child._value, computeFontPosition(child, child._value), child._fontColor, child._fontSize);
    } else {
        LIA_error_f("unknown gui object type of {}", static_cast<int>(child._type));
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
            std::vector<GuiObject>& gChildren = _childrenMap[child._id];
            for (GuiObject& gChild: gChildren) {
                passChild(gChild, scene, font);
            }
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
        scene->addSquare("header", oHeader._position, rotation, oHeader._scale, _isHeaderHover ? _style.headerHoverColor : _headerColor);
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
        if (!child._isHovered) {
            continue;
        }
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
        }  else if (child._type == GuiObjectType::GRID) {
            for (GuiObject& ch: _childrenMap[child._id]) {
                if (!ch._isHovered) {
					continue;
				}
                if (ch._type == GuiObjectType::BUTTON) {
                    LIA_debug(std::vformat("Mouse click {}", std::make_format_args(ch._id)));
                    ButtonEvent buttonEvent(
                        std::vformat("{}_{}", std::make_format_args(_id, ch._id)),
                        ch._action,
                        ch._arg0,
                        _id
                    );
                    eventManager->handleEvent(buttonEvent);
                    return true;
                }
            }
        }
    }
    return false;
}

void LIA::Window::mouseLastPosition(Position& pos) {
    _mouseLastHoverPos = pos;
}

void LIA::Window::removeHover() {
    if (!_visible || !_isHovered) {
        return;
    }
    _isHovered = false;
    _isHeaderHover = false;
    for (GuiObject& child : _children) {
        if (child._type == GuiObjectType::BUTTON || child._type == GuiObjectType::CHECKBOX) {
            child._isHovered = false;
        }
        else if (child._type == GuiObjectType::GRID) {
            child._isHovered = false;
            for (GuiObject& ch : _childrenMap[child._id]) {
                if (ch._type != GuiObjectType::BUTTON) {
                    continue;
                }
                ch._isHovered = false;
            }
        }
    }
}

bool LIA::Window::mouseHover(Position& pos) {
    if (!_visible) {
        return false;
    }
    bool found = false;
    _isHeaderHover = false;
    _isHovered = false;
    for (GuiObject& child : _children) {
        if (child._type == GuiObjectType::BUTTON || child._type == GuiObjectType::CHECKBOX) {
            if (isInRange2D(pos, child._position, child._scale)) {
                child._isHovered = true;
                found = true;
            } else {
                child._isHovered = false;
            }
        }
        else if (child._type == GuiObjectType::GRID) {
            child._isHovered = false;
            for (GuiObject& ch: _childrenMap[child._id]) {
                if (ch._type != GuiObjectType::BUTTON) {
                    continue;
                }
                if (isInRange2D(pos, ch._position, ch._scale)) {
                    ch._isHovered = true;
                    child._isHovered = true;
                    found = true;
                } else {
                    ch._isHovered = false;
                }    
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
            _isHovered = true;
            return true;
        }
    }
    if (!found) {
        found = isInRange2D(pos, _position, _scale);
    }
    _isHovered = found;
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

LIA::ALIGN LIA::Window::resolve(std::string al) {
    if (al.compare("none") == 0) {
        return ALIGN::NONE;
    }
    else if (al.compare("right") == 0) {
        return ALIGN::RIGHT;
    }
    else if (al.compare("center") == 0) {
        return ALIGN::CENTER;
    }
    return ALIGN::NONE;
}

void LIA::Window::setInitAlign(std::string initAlign) {
    _initAlign = resolve(initAlign);
}
void LIA::Window::setMovable(bool movable) {
    _isMovable = movable;
}