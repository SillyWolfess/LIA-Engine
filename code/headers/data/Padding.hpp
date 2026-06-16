#ifndef LIA_PADDING_HPP
#define LIA_PADDING_HPP
namespace LIA
{
    /**
     * float left
     * float right
     * float top
     * float bottom
     */
    struct Padding {
        float left;
        float right;
        float top;
        float bottom;
    };
    void copyPadding(Padding&, Padding&);
    Padding emptyPadding();
}
#endif