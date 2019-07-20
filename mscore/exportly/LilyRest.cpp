#include "LilyRest.hpp"

#include "libmscore/rest.h"
#include "mscore/exportly.hpp"

using namespace Ms;

LilyRest::LilyRest(const Rest* rest) : LilyElement(LILY_REST), _fraction(rest->ticks().reduced())
{
}

LilyRest::LilyRest(const Fraction& fraction) : LilyElement(LILY_REST), _fraction(fraction)
{
}

std::ofstream& LilyRest::operator>>(std::ofstream& file) const
{
    file << "r" << LilyExporter::lilyDuration(_fraction);
    return file;
}

Fraction LilyRest::getFraction() const
{
    return _fraction;
}

void LilyRest::merge(const LilyRest* other)
{
    _fraction += other->_fraction;
    _fraction.reduce();
}

bool LilyRest::isFullMeasureRest(const Fraction& timeSig)
{
    return (_fraction == timeSig);
}

void LilyRest::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : silence de " << _fraction.numerator() << " / "
              << _fraction.denominator() << std::endl;
}
