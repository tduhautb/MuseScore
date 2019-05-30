#include "exportly/LilyMeasure.hpp"
#include "exportly.hpp"
#include "exportly/LilyClef.hpp"
#include "exportly/LilyNote.hpp"
#include "exportly/LilyRest.hpp"
#include "exportly/LilyTimeSig.hpp"

#include "libmscore/chord.h"

using namespace Ms;

LilyMeasure::LilyMeasure(unsigned int num) : LilyElement()
{
    _first = nullptr;
    _measureNum = num;
    _anacrousis = false;
}

std::ofstream& LilyMeasure::operator>>(std::ofstream& file) const
{
    file << "\t";

    bool firstNote = true;

    for (LilyElement* element = _first; element; element = element->next())
    {
        if (_anacrousis && firstNote &&
            (dynamic_cast<LilyNote*>(element) || dynamic_cast<LilyRest*>(element)))
        {
            file << printAnacrousis();
            firstNote = false;
        }

        *element >> file << " ";
    }
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

void LilyMeasure::checkAnacrousis(const LilyTimeSig* timeSig)
{
    // an anacrousis must be the first measure
    if (_measureNum != 1)
        return;

    Fraction globalFraction;
    Fraction measureFraction = timeSig->getFraction();

    // get the duration of the whole measure
    for (LilyElement* element = _first; element; element = element->next())
    {
        if (dynamic_cast<LilyNote*>(element) || dynamic_cast<LilyRest*>(element))
            globalFraction += element->getFraction();
    }

    globalFraction.reduce();

    // compare it with the time signature and deduce the anacrousis
    if (globalFraction != measureFraction)
    {
        _anacrousis = true;
        _anacrousisFraction = globalFraction;
    }
}

std::string LilyMeasure::printAnacrousis() const
{
    if (!_anacrousis)
        return "";

    std::string anacrousis = "\\partial " + LilyExporter::lilyDuration(_anacrousisFraction) + " ";

    return anacrousis;
}
