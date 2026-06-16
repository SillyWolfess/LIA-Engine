#ifndef LIA_COLOR
#define LIA_COLOR

namespace LIA {
    struct Color {
        float r, g, b, a;
        Color() = default;
        Color(const Color&) = default;
        Color(float r, float g, float b) { this->r = r; this->g = g; this->b = b; this->a = 1.0f; };
    };
    void copyColor(Color& target, Color& source);
    Color whiteColor();
    Color blackColor();
    inline Color greenColor() { return Color(0.0f, 1.0f, 0.0f); };
    inline Color blueColor() { return Color(0.0f, 0.0f, 1.0f); };
}
#endif