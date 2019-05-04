#include "exportly/LilyMeasure.hpp"
#include "LilyClef.hpp"

using namespace Ms;

LilyMeasure::LilyMeasure(unsigned int num) : LilyElement()
{
    _first = nullptr;
    _measureNum = num;
}

std::ofstream& LilyMeasure::operator>>(std::ofstream& file) const
{
    file << "\t";
    for (LilyElement* element = _first; element; element = element->next())
        *element >> file << " ";
    file << "| % " << std::to_string(_measureNum) << std::endl;
    return file;
}

void LilyMeasure::addElement(LilyElement* element)
{
    if (!element)
        return;

    if (!_first)
    {
        _first = element;
        _current = element;
    }
    else
    {
        _current->setNext(element);
        element->setPrev(_current);
        _current = _current->next();
    }
}

void LilyMeasure::disconnectElement(const LilyElement* element)
{
    LilyElement* prev = element->prev();
    LilyElement* next = element->next();

    if (prev)
        prev->setNext(next);

    if (next)
        next->setPrev(prev);

    if (element == _first)
        _first = next;
}
