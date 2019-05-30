#include "LilyRest.hpp"

#include "libmscore/rest.h"
#include "mscore/exportly.hpp"

using namespace Ms;

LilyRest::LilyRest(const Rest* rest) : LilyElement(), _rest(rest)
{
    nbFullMeasures = 0;

    if (rest->durationType().isMeasure())
        nbFullMeasures++;
}

std::ofstream& LilyRest::operator>>(std::ofstream& file) const
{
    file << (_rest->durationType().isMeasure() ? "R" : "r");
    file << LilyExporter::lilyDuration(_rest->ticks());
    if (nbFullMeasures > 0)
        file << "*" << std::to_string(nbFullMeasures);

    return file;
}

Fraction LilyRest::getFraction() const
{
    return _rest->ticks();
}
