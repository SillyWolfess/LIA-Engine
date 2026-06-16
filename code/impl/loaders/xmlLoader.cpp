#include "loaders/xmlLoader.hpp"
#include "logs.hpp"
#include "tools/macros.hpp"

#include <iostream>

LIA::XmlLoader::XmlData LIA::XmlLoader::load(std::string path) {
    LIA_trace(std::vformat("Loading {} starting", std::make_format_args(path)));
    XmlData xmlData;

    rapidxml::file<> xmlFile(path.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());
    rapidxml::xml_node<> *root = doc.first_node();

    LIA_TRY
    parseNode(xmlData, root, "");
    LIA_CATCH(LIA_error("Failed to parse root node");)
    
    LIA_trace(std::vformat("Loading {} finished", std::make_format_args(path)));
    return xmlData;
}

void LIA::XmlLoader::parseNode(XmlData& xmlData, rapidxml::xml_node<> * root, std::string parentName) {
    for (rapidxml::xml_node<> *child = root->first_node(); child; child = child->next_sibling()) {
        rapidxml::node_type type = child->type();
        std::string chName = child->name();
        if (chName.compare("") == 0) {
            continue;
        }
        if (type == rapidxml::node_type::node_data) {
            LIA_trace_f("Adding value for '{}'", child->name());
            xmlData.values.emplace(child->name(), child->value());
        } else if (type == rapidxml::node_type::node_element) {
            LIA_trace_f("Adding value for '{}'", child->name());
            xmlData.values.emplace(child->name(), child->value());
        } else {
            std::string cType = "unknown";
            if (type == rapidxml::node_type::node_data) {
                cType = "data";
            } else if (type == rapidxml::node_type::node_element) {
                cType = "element";
            } else if (type == rapidxml::node_type::node_cdata) {
                cType = "cData";
            } else if (type == rapidxml::node_type::node_comment) {
                cType = "comment";
            } else if (type == rapidxml::node_type::node_declaration) {
                cType = "declaration";
            } else if (type == rapidxml::node_type::node_doctype) {
                cType = "docType";
            } else if (type == rapidxml::node_type::node_document) {
                cType = "document";
            } else if (type == rapidxml::node_type::node_pi) {
                cType = "pi";
            }
            LIA_trace_f("{}.type = {}", child->name(), cType);
        }
        XmlNode xmlNode;
        xmlNode.name = child->name();
        xmlNode.value = child->value();
        for (rapidxml::xml_attribute<> *attr = child->first_attribute(); attr; attr = attr->next_attribute())
        {
            xmlNode.attributes.emplace(attr->name(), attr->value());
        }
        if (parentName.compare("") != 0) {
            xmlData.nodes.at(parentName).children.emplace_back(xmlNode);
        } else {
            LIA_trace(std::vformat("Adding node {}", std::make_format_args(xmlNode.name)));
            xmlData.nodes.emplace(xmlNode.name, xmlNode);
            if (type == rapidxml::node_type::node_element) {
                LIA_trace(std::vformat("Adding children of '{}'", std::make_format_args(xmlNode.name)));
                LIA_TRY
                parseNode(xmlData, child, xmlNode.name);
                LIA_CATCH(LIA_error(std::vformat("Failed to parse node {}", std::make_format_args(xmlNode.name)));)
            }
        }
    }
}

void LIA::XmlLoader::printNode(rapidxml::xml_node<> *node) {
    rapidxml::node_type type = node->type();
    LIA_debug_f("node.name = {}; node.attr = [", node->name());
    for (rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
    {
        LIA_debug_f("{} = {}", attr->name(), attr->value());
    }
    LIA_debug("]");
    if (type == rapidxml::node_type::node_data) {
        LIA_debug_f("node.value = {}", node->value());
    }

    for (rapidxml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
        printNode(child);
    }
}

LIA::XmlLoader::XmlNode LIA::XmlLoader::getNode(XmlData& data, std::string name) {
    return data.nodes.at(name);
}

std::string LIA::XmlLoader::getValue(XmlData& data, std::string name) {
    return data.values.at(name);
}

std::string LIA::XmlLoader::getValue(XmlData& data, std::string name, std::string defaultValue) {
    if (data.values.find(name) == data.values.end()) {
        return defaultValue;
    }
    return data.values.at(name);
}

bool LIA::XmlLoader::getBoolean(XmlData& data, std::string name) {
    return boolean(getValue(data, name));
}
bool LIA::XmlLoader::getBoolean(XmlData& data, std::string name, bool defaultValue) {
    return boolean(getValue(data, name, defaultValue ? "true" : "false"));
}
char LIA::XmlLoader::getChar(XmlData& data, std::string name) {
    return makeChar(getValue(data, name));
}
char LIA::XmlLoader::getChar(XmlData& data, std::string name, char defaultValue) {
    return makeChar(getValue(data, name, "" + defaultValue));
}
int LIA::XmlLoader::getInt(XmlData& data, std::string name) {
    return integer(getValue(data, name));
}
int LIA::XmlLoader::getInt(XmlData& data, std::string name, int defaultValue) {
    return integer(getValue(data, name, std::to_string(defaultValue)));
}
float LIA::XmlLoader::getFloat(XmlData& data, std::string name) {
    return makeFloat(getValue(data, name));
}
float LIA::XmlLoader::getFloat(XmlData& data, std::string name, float defaultValue) {
    return makeFloat(getValue(data, name, std::to_string(defaultValue)));
}
std::string LIA::XmlLoader::getString(XmlData& data, std::string name) {
    return getValue(data, name);
}
std::string LIA::XmlLoader::getString(XmlData& data, std::string name, std::string defaultValue) {
    return getValue(data, name, defaultValue);
}
LIA::Color LIA::XmlLoader::getColor(XmlData& data, std::string name) {
    XmlNode node = getNode(data, name);
    return getColor(node);
}

std::string LIA::XmlLoader::getAttribute(XmlNode& node, std::string attribute) {
    return node.attributes.at(attribute);
}

std::string LIA::XmlLoader::getAttribute(XmlNode& node, std::string attribute, std::string defaultValue) {
    if (node.attributes.find(attribute) == node.attributes.end()) {
        return defaultValue;
    }
    return node.attributes.at(attribute);
}

bool LIA::XmlLoader::getBoolean(XmlNode& node, std::string attribute) {
    return boolean(getAttribute(node, attribute));
}
bool LIA::XmlLoader::getBoolean(XmlNode& node, std::string attribute, std::string defaultValue) {
    return boolean(getAttribute(node, attribute, defaultValue));
}
bool LIA::XmlLoader::getBoolean(XmlNode& node, std::string attribute, bool defaultValue) {
    return boolean(getAttribute(node, attribute, defaultValue ? "true" : "false"));
}
int LIA::XmlLoader::getInt(XmlNode& node, std::string attribute) {
    return integer(getAttribute(node, attribute));
}
int LIA::XmlLoader::getInt(XmlNode& node, std::string attribute, int fallback) {
    std::map<std::string, std::string>::iterator it = node.attributes.find(attribute);
    if (it == node.attributes.end()) {
        return fallback;
    }
    return integer(it->second);
}
float LIA::XmlLoader::getFloat(XmlNode& node, std::string attribute) {
    return makeFloat(getAttribute(node, attribute));
}
float LIA::XmlLoader::getFloat(XmlNode& node, std::string attribute, float fallback) {
    std::map<std::string, std::string>::iterator it = node.attributes.find(attribute);
    if (it == node.attributes.end()) {
        return fallback;
    }
    return makeFloat(it->second);
}
std::string LIA::XmlLoader::getString(XmlNode& node, std::string attribute) {
    return getAttribute(node, attribute);
}

std::string LIA::XmlLoader::getString(XmlNode& node, std::string attribute, std::string defaultValue) {
    return getAttribute(node, attribute, defaultValue);
}

LIA::Rotation LIA::XmlLoader::getRotation(XmlNode& node) {
    Rotation rotation;
    rotation.x = getFloat(node, "x");
    rotation.y = getFloat(node, "y");
    rotation.z = getFloat(node, "z", 0.0);
    return rotation;
}

LIA::Padding LIA::XmlLoader::getPadding(XmlNode& node) {
    Padding padding;
    padding.left = getFloat(node, "left", 0.0f);
    padding.right = getFloat(node, "right", 0.0f);
    padding.top = getFloat(node, "top", 0.0f);
    padding.bottom = getFloat(node, "bottom", 0.0f);
    return padding;
}

LIA::Direction LIA::XmlLoader::getDirection(XmlNode& node) {
    Direction direction;
    direction.x = getFloat(node, "x");
    direction.y = getFloat(node, "y");
    direction.z = getFloat(node, "z");
    return direction;
}

LIA::Position LIA::XmlLoader::getPosition(XmlNode& node) {
    Position position;
    position.x = getFloat(node, "x");
    position.y = getFloat(node, "y");
    position.z = getFloat(node, "z", 0.0f);
    return position;
}

LIA::Position LIA::XmlLoader::getPosition2D(XmlNode& node) {
    Position position;
    position.x = getFloat(node, "x");
    position.y = getFloat(node, "y");
    position.z = 0.0f;
    return position;
}

LIA::Scale LIA::XmlLoader::getScale2D(XmlNode& node) {
    Scale scale;
    scale.x = getFloat(node, "x");
    scale.y = getFloat(node, "y");
    scale.z = 1.0f;
    return scale;
}

LIA::Scale LIA::XmlLoader::getScale2D(XmlData& data, std::string name) {
    XmlNode node = getNode(data, name);
    return getScale2D(node);
}

LIA::Scale LIA::XmlLoader::getScale(XmlNode& node) {
    Scale scale;
    scale.x = getFloat(node, "x");
    scale.y = getFloat(node, "y");
    scale.z = getFloat(node, "z", 1.0f);
    return scale;
}

LIA::Color LIA::XmlLoader::getColor(XmlNode& node) {
    Color color;
    color.r = getFloat(node, "r");
    color.g = getFloat(node, "g");
    color.b = getFloat(node, "b");
    color.a = getFloat(node, "a");
    return color;
}

float LIA::XmlLoader::getSize(XmlNode& node) {
    return getFloat(node, "size");
}

LIA::Int2D LIA::XmlLoader::getInt2D(XmlNode& node) {
    Int2D number;
    number.x = getInt(node, "x");
    number.y = getInt(node, "y");
    return number;
}

std::string LIA::XmlLoader::getId(XmlNode& node) {
    return getString(node, "id");
}

LIA::Padding LIA::XmlLoader::getPadding(XmlData& data) {
    XmlNode node = getNode(data, "padding");
    return getPadding(node);
}

LIA::Position LIA::XmlLoader::getPosition(XmlData& data) {
    XmlNode node = getNode(data, "position");
    return getPosition(node);
}

LIA::Position LIA::XmlLoader::getPosition(XmlData& data, std::string nodeName) {
    XmlNode node = getNode(data, nodeName);
    return getPosition(node);
}

LIA::Rotation LIA::XmlLoader::getRotation(XmlData& data) {
    XmlNode node = getNode(data, "rotation");
    return getRotation(node);
}

LIA::Direction LIA::XmlLoader::getDirection(XmlData& data) {
    XmlNode node = getNode(data, "direction");
    return getDirection(node);
}

LIA::Scale LIA::XmlLoader::getScale(XmlData& data) {
    XmlNode node = getNode(data, "scale");
    return getScale(node);
}

std::string LIA::XmlLoader::getId(XmlData& data) {
    return data.values.at("id");
}
