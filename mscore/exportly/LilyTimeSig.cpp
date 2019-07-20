#include "LilyTimeSig.hpp"

#include "libmscore/timesig.h"

using namespace Ms;

LilyTimeSig::LilyTimeSig(const TimeSig* timeSig) : LilyElement(LILY_TIMESIG), _timeSig(timeSig)
{
}

LilyTimeSig::LilyTimeSig(const LilyTimeSig* otherSig) : LilyElement(LILY_TIMESIG)
{
    _timeSig = otherSig->_timeSig;
}

std::ofstream& LilyTimeSig::operator>>(std::ofstream& file) const
{
    file << "\t\\time " << std::to_string(_timeSig->numerator()) << "/"
         << std::to_string(_timeSig->denominator()) << std::endl;

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

Fraction LilyTimeSig::getFraction() const
{
    return Fraction(_timeSig->numerator(), _timeSig->denominator());
}

void LilyTimeSig::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _timeSig->accessibleInfo().toStdString() << std::endl;
}
