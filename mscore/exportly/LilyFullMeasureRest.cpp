#include "exportly/LilyFullMeasureRest.hpp"
#include "exportly.hpp"

using namespace Ms;

LilyFullMeasureRest::LilyFullMeasureRest(const Fraction& fraction, unsigned int measureNum)
    : LilyElement(LILY_FULLMEASUREREST), _nbFullMeasures(1), _numFirstMeasure(measureNum),
      _fraction(fraction)
{
}

std::ofstream& LilyFullMeasureRest::operator>>(std::ofstream& file) const
{
    file << "\tR";

    if (_fraction.numerator() > _fraction.denominator())
    {
        // special case for irregular bars such as 5/4, a full measure rest cannot be described by
        // the combination of plain durations and dots. To handle this with lilypond, we have to
        // create a full measure rest based on the numerator and the denominator
        file << _fraction.denominator() << "*" << _fraction.numerator();
    }
    else
    {
        file << LilyExporter::lilyDuration(_fraction);
    }

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

Fraction LilyFullMeasureRest::getFraction() const
{
    return _fraction;
}
