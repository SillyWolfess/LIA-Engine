#ifndef LIA_XML_LOADER
#define LIA_XML_LOADER
#include "parsers/xml/rapidxml.hpp"
#include "parsers/xml/rapidxml_utils.hpp"
#include "data/position.hpp"
#include "data/color.hpp"
#include "data/Padding.hpp"

#include <string>
#include <map>
#include <list>
namespace LIA {
    class XmlLoader {
        public:
            struct XmlNode;
            struct XmlNode {
                std::string name;
                std::string value;
                std::map<std::string, std::string> attributes;
                std::list<XmlNode> children;
            };
            struct XmlData {
                std::map<std::string, std::string> values;
                std::map<std::string, XmlNode> nodes;
            };
            static void parseNode(XmlData&, rapidxml::xml_node<> * root, std::string);

            static XmlData load(std::string);
            static void printNode(rapidxml::xml_node<> *);
            bool boolean(std::string value) { return value == "true"; }
            int integer(std::string value) { return std::stoi(value); }
            float makeFloat(std::string value) { return std::stof(value); }
            char makeChar(std::string value) { return value.length() == 0 ? '\0' : value[0]; }
            
            XmlNode getNode(XmlData&, std::string);
            std::string getAttribute(XmlNode&, std::string);
            std::string getAttribute(XmlNode&, std::string, std::string);
            std::string getValue(XmlData&, std::string);
            std::string getValue(XmlData&, std::string, std::string);

            std::string getValue(XmlNode&);
            int getInt(XmlNode&);

            bool getBoolean(XmlData&, std::string);
            bool getBoolean(XmlData&, std::string, bool);
            char getChar(XmlData&, std::string);
            char getChar(XmlData&, std::string, char);
            int getInt(XmlData&, std::string);
            int getInt(XmlData&, std::string, int);
            float getFloat(XmlData&, std::string);
            float getFloat(XmlData&, std::string, float);
            std::string getString(XmlData&, std::string);
            std::string getString(XmlData&, std::string, std::string);
            Color getColor(XmlData&, std::string);

            bool getBoolean(XmlNode&, std::string);
            bool getBoolean(XmlNode&, std::string, std::string);
            bool getBoolean(XmlNode&, std::string, bool);
            int getInt(XmlNode&, std::string);
            int getInt(XmlNode&, std::string, int);
            float getFloat(XmlNode&, std::string);
            float getFloat(XmlNode&, std::string, float);
            std::string getString(XmlNode&, std::string);
            std::string getString(XmlNode&, std::string, std::string);

            Position getPosition(XmlNode&);
            Position getPosition(XmlData&);
            Position getPosition(XmlData&, std::string);
            Position getPosition2D(XmlNode&);

            Direction getDirection(XmlNode&);
            Direction getDirection(XmlData&);

            Padding getPadding(XmlNode&);
            Padding getPadding(XmlData&);
            
            Rotation getRotation(XmlNode&);
            Rotation getRotation(XmlData&);

            Scale getScale2D(XmlData&, std::string);
            Scale getScale2D(XmlNode&);
            Scale getScale(XmlNode&);
            Scale getScale(XmlData&);

            Color getColor(XmlNode&);
            Int2D getInt2D(XmlNode&);

            float getSize(XmlNode&);
            std::string getId(XmlNode&);
            std::string getId(XmlData&);
    };
}
#endif