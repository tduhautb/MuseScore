#include "exportly/LilyTuplet.hpp"

#include "libmscore/tuplet.h"

using namespace Ms;

LilyTuplet::LilyTuplet(const Tuplet* tuplet) : LilyElement(LILY_TUPLET)
{
    _ratio = tuplet->ratio();
}

std::ofstream& LilyTuplet::operator>>(std::ofstream& file) const
{
    file << "\\tuplet " << _ratio.numerator() << "/" << _ratio.denominator() << " {";
    for (LilyElement* elem : _elements)
    {
        *elem >> file;
        file << " ";
    }
    file << "} ";
    return file;
}

void LilyTuplet::addElement(LilyElement* element)
{
    _elements.push_back(element);
    _cumulatedFraction += element->getFraction();
}

Fraction LilyTuplet::getFraction() const
{
    return _cumulatedFraction / _ratio;
}

void LilyTuplet::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << "LilyTuplet " << _ratio.numerator() << "/" << _ratio.denominator() << std::endl;

    for (LilyElement* element : _elements)
        element->log(indentation + 1);
}
