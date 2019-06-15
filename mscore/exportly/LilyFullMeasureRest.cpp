#include "exportly/LilyFullMeasureRest.hpp"
#include "exportly.hpp"

using namespace Ms;

LilyFullMeasureRest::LilyFullMeasureRest(const Fraction& fraction, unsigned int measureNum)
    : LilyElement(LILY_FULLMEASUREREST), _nbFullMeasures(0), _numFirstMeasure(measureNum),
      _fraction(fraction)
{
}

std::ofstream& LilyFullMeasureRest::operator>>(std::ofstream& file) const
{
    file << "\tR" << LilyExporter::lilyDuration(_fraction);

    if (_nbFullMeasures > 1)
        file << "*" << std::to_string(_nbFullMeasures);

    file << " | % " << _numFirstMeasure;

    if (_nbFullMeasures > 1)
        file << " - " << _numFirstMeasure + _nbFullMeasures - 1;

    file << std::endl;

    return file;
}

void LilyFullMeasureRest::addFullMeasure(unsigned int nb)
{
    _nbFullMeasures += nb;
}

void LilyFullMeasureRest::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _nbFullMeasures << " mesures de silence "
              << _fraction.numerator() << " / " << _fraction.denominator() << std::endl;
}
