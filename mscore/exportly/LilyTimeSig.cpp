#include "LilyTimeSig.hpp"

#include "libmscore/timesig.h"

using namespace Ms;

LilyTimeSig::LilyTimeSig(const TimeSig* timeSig) : LilyElement(), _timeSig(timeSig)
{
}

std::ofstream& LilyTimeSig::operator>>(std::ofstream& file) const
{
    file << std::endl;
    file << "\t\\time " << std::to_string(_timeSig->numerator()) << "/"
         << std::to_string(_timeSig->denominator()) << std::endl
         << "\t";

    return file;
}

bool LilyTimeSig::operator==(const LilyTimeSig& other) const
{
    return _timeSig->globalSig() == other._timeSig->globalSig();
}

bool LilyTimeSig::operator!=(const LilyTimeSig& other) const
{
    return !(*this == other);
}
