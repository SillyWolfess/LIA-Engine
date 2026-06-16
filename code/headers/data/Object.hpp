#ifndef LIA_OBJECT
#define LIA_OBJECT

#include "position.hpp"
#include "./../precompiled.hpp"
#include <string>

namespace LIA {
    struct ModelInfo {
        std::string name;
        int id;
        /*
        std::string shader;
        GLuint vao;
        int size;
        bool hasIndices;
        bool loaded = false;
        bool infoLoaded = false;
        */
        #if LIA_DEBUG_ON
        ModelInfo();
        ModelInfo(const ModelInfo&);
        ~ModelInfo();
        #endif
    };
    enum ObjectType {OBJECT, PLAYER};
    struct PhysicData {
        float _mass = 0;
        Speed _force;
    };
    struct MovementData {
        Speed _speed;
        Speed _direction;
    };
    struct CollisionData {
        bool _grounded = true;
    };
    struct Object {
        bool _hide = false;
        std::string _name;
        std::string _materialLib;
        ModelInfo _modelInfo;
        Position _position;
        Rotation _rotation;

        MovementData _movement;
        PhysicData _physics;
        CollisionData _collison;

        Scale _scale;
        ObjectType _type;

        #if LIA_DEBUG_ON
        Object();
        Object(const Object&);
        ~Object();
        #endif
    };
}
#endif