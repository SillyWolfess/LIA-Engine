#include "datastructure/grid.hpp"
#include "precompiled.hpp"

void LIA::Grid::setPosition(Position& position) {
    _position.x = position.x;
    _position.y = position.y;
    _position.z = position.z;
}

void LIA::Grid::setScale(Scale& scale) {
    _scale.x = scale.x;
    _scale.y = scale.y;
    _scale.z = scale.z;
}

void LIA::Grid::setDimension(int x, int y) {
    _dX = x;
    _dY = y;
}

bool LIA::Grid::load(ObjectManager& objectManager) {
    LIA_TRY
        int oIndx = 0;
        int lSize = _objectDef.size();
        for (int y = 0; y < _dY; y++) {
            for (int x = 0; x < _dX; x++) {    
                if (oIndx > lSize - 1) {
                    oIndx = 0;
                }
                int indx = objectManager.create(_objectDef[oIndx]);
                Object* obj = objectManager.get(indx);
                objectManager.setPosition(obj, ((x + 1) * _scale.x) + _position.x, ((y + 1) * _scale.y) + _position.y, 0);
                objectManager.setRotation(obj, 0);
                objectManager.setScale(obj, _scale.x / 2, _scale.y /2, 1.0f);
                oIndx++;
            }
            if (oIndx > lSize - 1) {
                oIndx = 0;
            }
            if (_dX % 2 == 0) {
                oIndx++;
            }
        }
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool LIA::Grid::init(std::string path) {
    LIA_TRY
        XmlLoader xmlLoader;
        XmlLoader::XmlData xmlGridData = xmlLoader.load(path);
        XmlLoader::XmlNode xmlPosition = xmlLoader.getNode(xmlGridData, "position");
        XmlLoader::XmlNode xmlScale = xmlLoader.getNode(xmlGridData, "scale");
        XmlLoader::XmlNode xmlDim = xmlLoader.getNode(xmlGridData, "dim");
        
        _position = xmlLoader.getPosition2D(xmlPosition);
        _scale = xmlLoader.getScale2D(xmlScale);
        Int2D dim = xmlLoader.getInt2D(xmlDim);
        _dX = dim.x;
        _dY = dim.y;

        XmlLoader::XmlNode xmlObjects = xmlLoader.getNode(xmlGridData, "objects");
        int indx = 0;
        for(auto object: xmlObjects.children) {
            std::string oName = xmlLoader.getString(object, "object");
            LIA_trace(std::vformat("Adding {} at index {}", std::make_format_args(oName, indx)));
            _objectDef.emplace(indx, oName);
            indx++;
        }
        return true;
    LIA_CATCH_RETURN_FALSE
}