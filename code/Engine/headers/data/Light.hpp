#ifndef LIA_LIGHT_HPP
#define LIA_LIGHT_HPP
#include "data/position.hpp"
#include "data/Color.hpp"
namespace LIA {
    enum LightType {DIRECTIONAL, POINT, FLASHLIGHT};
    struct Light {
        Position _position;
        Direction _direction;
        Color _color;
        LightType _type;
        float _linear;
        float _quadratic;
        float _cutoff;
        float _intensity;
    };
}
#endif