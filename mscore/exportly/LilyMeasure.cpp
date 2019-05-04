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

void LilyMeasure::moveKeyToNextMeasure()
{
    // we assume that there is at most one key in the measure
    // if the key is the first element in the measure, we ignore it and process the rest of the
    // measure
    LilyKey* key = nullptr;

    for (LilyElement* current = _first->next(); current && !key; current = current->next())
        key = dynamic_cast<LilyKey*>(current);

    // no key in the measure
    if (!key)
        return;

    // we have a key in the measure, we put it in the next one
    // in the case we don't have a measure next, the key is dropped
    disconnectElement(key);

    LilyMeasure* nextMeasure = dynamic_cast<LilyMeasure*>(next());
    if (!nextMeasure)
        return;

	// set the key as the first element of the next measure
    nextMeasure->_first->setPrev(key);
    key->setNext(nextMeasure->_first);
    nextMeasure->_first = key;
}
