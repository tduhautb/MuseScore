#include "LilyElement.hpp"

using namespace Ms;

LilyElement::LilyElement(LilyType type)
{
    _prev = nullptr;
    _next = nullptr;
    _type = type;
}

void LilyElement::setNext(LilyElement* next)
{
    _next = next;
}

void LilyElement::setPrev(LilyElement* prev)
{
    _prev = prev;
}

LilyElement* LilyElement::next() const
{
    return _next;
}

LilyElement* LilyElement::prev() const
{
    return _prev;
}

Fraction LilyElement::getFraction() const
{
    return Fraction();
}

LilyType LilyElement::getType() const
{
    return _type;
}
