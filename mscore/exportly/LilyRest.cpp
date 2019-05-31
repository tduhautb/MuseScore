#include "LilyRest.hpp"

#include "libmscore/rest.h"
#include "mscore/exportly.hpp"

using namespace Ms;

LilyRest::LilyRest(const Rest* rest) : LilyElement(LILY_REST), _fraction(rest->ticks().reduced())
{
    _fullMeasureRest = false;
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

void LilyRest::checkFullMeasureRest(const Fraction& timeSig)
{
    _fullMeasureRest = (_fraction == timeSig);
}

bool LilyRest::isFullMeasure() const
{
    return _fullMeasureRest;
}
