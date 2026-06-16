#ifndef LIA_POSITION
#define LIA_POSITION

namespace LIA {
    struct LIAV3 {
        float x, y, z;
    };
    struct LIAV2 {
        float x, y;
    };
    struct Speed : LIAV3 {
    };
    struct Position : LIAV3 {
    };
    struct Direction : LIAV3 {
    };
    struct Rotation : LIAV3 {
    };
    struct Scale : LIAV3 {
    };
    struct Int2D {
        int x, y;
    };
    void copy(LIAV3& target, const LIAV3& source);

    bool isInRange2D(const LIAV3& pos, const LIAV3& target, const LIAV3& scale);
    void applyForceByAngleXZ(LIAV3& vec, float angle, float force);
    void setZero(LIAV3& target);
    void setOne(LIAV3& target);
    void setAll(LIAV3& target, float);
    Position emptyPosition();
    Rotation emptyRotation();
    Rotation convertRotation(Rotation&);
    Scale emptyScale();
    Speed emptySpeed();
}
#endif