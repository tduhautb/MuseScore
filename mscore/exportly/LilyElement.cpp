#include "LilyElement.hpp"

using namespace Ms;

LilyElement::LilyElement()
{
    _prev = nullptr;
    _next = nullptr;
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
