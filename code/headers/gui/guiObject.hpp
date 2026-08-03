#ifndef LIA_GUI_OBJECT
#define LIA_GUI_OBJECT
#include <string>
#include "data/position.hpp"
#include "data/Color.hpp"

namespace LIA {
    /**
     * GuiObjectType type
     * string id
     * string type
     * string value
     * string placeholder
     * string label
     * string action - for event
     * string arg0 - for event
     * Position position
     * Scale scale
     * Color fontColor
     * Color bgColor - background color
     * int fontSize
     * bool isHovered
     */
    enum GUI_LABEL_POSITION {PREFIX, SUFFIX};
    enum GuiObjectType {BUTTON, FIELD, CHECKBOX, LABEL, GRID, LIST, OPTIONS};
    struct GridData {
        int x, y;
    };
    struct GuiObject {
        GuiObjectType _type;
        GridData _grid;
        GUI_LABEL_POSITION _labelAlignment = GUI_LABEL_POSITION::PREFIX;
        std::string _id;
        std::string _value;
        std::vector<std::string> _values;
        std::string _texture;
        int _index;
        bool _valueB;
        std::string _placeholder;
        std::string _label;
        
        std::string _action;
        std::string _arg0;
        std::string _tooltip;

        Position _position;
        Scale _scale;
        Color _fontColor;
        Color _bgColor;
        Color _disabledBgColor;
        int _fontSize;
        bool _isHovered;
        bool _enabled;
    };
    inline std::string objectTypeToString(GuiObjectType& object) {
        if (object == GuiObjectType::BUTTON) {
            return "button";
        }
        if (object == GuiObjectType::FIELD) {
            return "field";
        }
        if (object == GuiObjectType::CHECKBOX) {
            return "checkbox";
        }
        if (object == GuiObjectType::LABEL) {
            return "label";
        }
        if (object == GuiObjectType::LIST) {
            return "list";
        }
        if (object == GuiObjectType::GRID) {
            return "grid";
        }
        return "--";
    }
}
#endif