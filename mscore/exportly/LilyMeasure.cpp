#include "exportly/LilyMeasure.hpp"
#include "exportly.hpp"
#include "exportly/LilyClef.hpp"
#include "exportly/LilyNote.hpp"
#include "exportly/LilyRest.hpp"
#include "exportly/LilyTimeSig.hpp"

#include "libmscore/chord.h"

using namespace Ms;

LilyMeasure::LilyMeasure(unsigned int num) : LilyElement(LILY_MEASURE)
{
    _first = nullptr;
    _measureNum = num;
    _anacrousis = false;
    _fullRest = false;
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

void LilyMeasure::checkAnacrousis()
{
    // an anacrousis must be the first measure
    if (_measureNum != 1)
        return;

    Fraction globalFraction;

    // get the duration of the whole measure
    for (LilyElement* element = _first; element; element = element->next())
    {
        if (dynamic_cast<LilyNote*>(element) || dynamic_cast<LilyRest*>(element))
            globalFraction += element->getFraction();
    }

    globalFraction.reduce();

    // compare it with the time signature and deduce the anacrousis
    if (globalFraction != _fraction)
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

void LilyMeasure::compressRests()
{
    LilyRest* lastRest = nullptr;

    for (LilyElement* element = _first; element; element = element->next())
    {
        LilyRest* tmpRest = dynamic_cast<LilyRest*>(element);

        if (lastRest && tmpRest)
        {
            lastRest->merge(tmpRest);
            disconnectElement(tmpRest);
            delete tmpRest;
        }
        else
        {
            lastRest = tmpRest;
        }

        if (lastRest)
            _fullRest = lastRest->isFullMeasureRest(_fraction);
    }
}

bool LilyMeasure::isFullBarRest() const
{
    return _fullRest;
}

Fraction LilyMeasure::getFraction() const
{
    return _fraction;
}

unsigned int LilyMeasure::getMeasureNum() const
{
    return _measureNum;
}

void LilyMeasure::setFraction(const Fraction& fraction)
{
    _fraction = fraction;
    _fraction.reduce();
}

void LilyMeasure::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << "Mesure " << getMeasureNum() << std::endl;

    for (LilyElement* current = _first; current; current = current->next())
        current->log(indentation + 1);
}
