#include "LilyRest.hpp"

#include "libmscore/rest.h"
#include "mscore/exportly.hpp"

using namespace Ms;

LilyRest::LilyRest(const Rest* rest) : LilyElement(LILY_REST), _fraction(rest->ticks().reduced())
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
