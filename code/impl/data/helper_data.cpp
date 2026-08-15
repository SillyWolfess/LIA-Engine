#include "data/position.hpp"
#include "data/Color.hpp"
#include "data/Event.hpp"
#include "data/Object.hpp"
#include "data/Model.hpp"
#include "data/ModelData.hpp"
#include "data/sceneObject.hpp"
#include "data/Padding.hpp"
#include "math/Math.hpp"

void LIA::applyForceByAngleXZ(LIAV3& vec, float angle, float force) {
    float fx = LIA::Math::sinOf(angle);
    float fz = LIA::Math::cosOf(angle);

    vec.x = force * fx;
    vec.z = force * fz;
}

bool LIA::isInRange2D(const LIAV3& pos, const LIAV3& target, const LIAV3& scale) {
    return  target.x < pos.x && pos.x < target.x + scale.x &&
            target.y < pos.y && pos.y < target.y + scale.y;
}

bool LIA::isInRangeCentered2D(const LIAV3& pos, const LIAV3& target, const LIAV3& scale) {
    return  target.x - (scale.x * 0.5f) < pos.x && pos.x < target.x + (scale.x * 0.5f) &&
        target.y - (scale.y * 0.5f) < pos.y && pos.y < target.y + (scale.y * 0.5f);
}

void LIA::copyPadding(Padding& target, Padding& source) {
    target.left = source.left;
    target.right = source.right;
    target.top = source.top;
    target.bottom = source.bottom;
}
void LIA::copy(LIAV3& target, const LIAV3& source) {
    target.x = source.x;
    target.y = source.y;
    target.z = source.z;
}
void LIA::setOne(LIAV3& target) {
    setAll(target, 1.0f);
}
void LIA::setZero(LIAV3& target) {
    setAll(target, 0.0f);
}
void LIA::setAll(LIAV3& target, float value) {
    target.x = value;
    target.y = value;
    target.z = value;
}
void LIA::copyColor(Color& target, Color& source) {
    target.r = source.r;
    target.g = source.g;
    target.b = source.b;
    target.a = source.a;
}
LIA::Position LIA::emptyPosition() {
    Position empty;
    setZero(empty);
    return empty;
}
LIA::Rotation LIA::emptyRotation() {
    Rotation empty;
    setZero(empty);
    return empty;
}
LIA::Rotation LIA::convertRotation(Rotation& rotation) {
    const float PiOver180 = 0.0174532925f;
    Rotation result;
    result.x = rotation.x * PiOver180;
    result.y = rotation.y * PiOver180;
    result.z = rotation.z * PiOver180;
    return result;
}
LIA::Scale LIA::emptyScale() {
    Scale scale;
    setOne(scale);
    return scale;
}
LIA::Speed LIA::emptySpeed() {
    Speed empty;
    setZero(empty);
    return empty;
}
LIA::Padding LIA::emptyPadding() {
    Padding empty;
    empty.bottom = 0.0f;
    empty.left = 0.0f;
    empty.right = 0.0f;
    empty.top = 0.0f;
    return empty;
}
LIA::Color LIA::whiteColor() {
    Color color;
    color.r = 1;
    color.g = 1;
    color.b = 1;
    color.a = 1;
    return color;
}
LIA::Color LIA::blackColor() {
    Color color;
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 1;
    return color;
}

#if LIA_DEBUG_ON
LIA::Object::Object() {
    LIA_DEBUG_CONSTRUCTOR_NO_LOG("Object");
}

LIA::Object::Object(const Object& object) {
    LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG("Object");
    _name = object._name;
    _modelInfo = object._modelInfo;
    _position = object._position;
    _rotation = object._rotation;
    _scale = object._scale;
    _type = object._type;
}

LIA::Object::~Object() {
    LIA_DEBUG_DESTRUCTOR_NO_LOG("Object");
}

LIA::ModelInfo::ModelInfo() {
    LIA_DEBUG_CONSTRUCTOR_NO_LOG("ModelInfo");
}

LIA::ModelInfo::ModelInfo(const ModelInfo& modelInfo) {
    LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG("ModelInfo");
    shader = modelInfo.shader;
    vao = modelInfo.vao;
    size = modelInfo.size;
    id = modelInfo.id;
    hasIndices = modelInfo.hasIndices;
    loaded = modelInfo.loaded;
}

LIA::ModelInfo::~ModelInfo() {
    LIA_DEBUG_DESTRUCTOR_NO_LOG("ModelInfo");
}

LIA::Model::Model() {
    LIA_DEBUG_CONSTRUCTOR_NO_LOG("Model");
}

LIA::Model::Model(const Model& model) {
    LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG("Model");
    data = model.data;
    folder = model.folder;
    path = model.path;
    name = model.name;
    shader = model.shader;

    vao = model.vao;
    vbo = model.vbo;
    program = model.program;

    size = model.size;
    hasIndices = model.hasIndices;
    isInGpu = model.isInGpu;
    indx = model.indx;
}

LIA::Model::~Model() {
    LIA_DEBUG_DESTRUCTOR_NO_LOG("Model");
}

#if LIA_GRAPHIC_OBJECT_DEBUG
    LIA::ModelData::ModelData() {
        LIA_DEBUG_CONSTRUCTOR_NO_LOG("ModelData");
    }

    LIA::ModelData::ModelData(const ModelData& object) {
        LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG("ModelData");
        _complete = object._complete;
        min_x = object.min_x;
        min_y = object.min_y;
        min_z = object.min_z;
        max_x = object.max_x;
        max_y = object.max_y;
        max_z = object.max_z;

        vertices.reserve(object.vertices.size());
        for (glm::vec3 v: object.vertices) {
            vertices.emplace_back(v);
        }
        uvs.reserve(object.uvs.size());
        for (glm::vec2 v: object.uvs) {
            uvs.emplace_back(v);
        }
        colours.reserve(object.colours.size());
        for (glm::vec3 v: object.colours) {
            colours.emplace_back(v);
        }
        normals.reserve(object.normals.size());
        for (glm::vec3 v: object.normals) {
            normals.emplace_back(v);
        }
        materialIds.reserve(object.materialIds.size());
        for (int id: object.materialIds) {
            materialIds.emplace_back(id);
        }
        materials.reserve(object.materials.size());
        for (gMaterial material: object.materials) {
            materials.emplace_back(material);
        }
    }

    LIA::ModelData::~ModelData() {
        LIA_DEBUG_DESTRUCTOR_NO_LOG("ModelData");
    }

    LIA::gMaterial::gMaterial() {
        LIA_DEBUG_CONSTRUCTOR_NO_LOG("gMaterial");
    }

    LIA::gMaterial::gMaterial(const gMaterial& material) {
        LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG("gMaterial");
        for (int i = 0; i < 3; i++) {
            Ka[i] = material.Ka[i];
            Kd[i] = material.Kd[i];
            Ks[i] = material.Ks[i];
            Ke[i] = material.Ke[i];
        }
        Ni = material.Ni;
        d = material.d;
        illum = material.illum;

        textureID = material.textureID;
        bumpID = material.bumpID;
        emID = material.emID;

        hasTexture = material.hasTexture;
        hasBump = material.hasBump;
        hasEm = material.hasEm;
    }

    LIA::gMaterial::~gMaterial() {
        LIA_DEBUG_DESTRUCTOR_NO_LOG("gMaterial");
    }
#endif

#if LIA_SCENE_OBJECT_DEBUG
    LIA::SceneObject::SceneObject() {
        LIA_DEBUG_CONSTRUCTOR_NO_LOG("SceneObject");
    }

    LIA::SceneObject::SceneObject(const SceneObject& object) {
        LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG("SceneObject");
        _position = object._position;
        _rotation = object._rotation;
        _scale = object._scale;
        _color = object._color;
        _vao = object._vao;
        _shader = object._shader;
        _size = object._size;
        _useIndices = object._useIndices;
        _passColor = object._passColor;
    }

    LIA::SceneObject::~SceneObject() {
        LIA_DEBUG_DESTRUCTOR_NO_LOG("SceneObject");
    }
#endif
#endif