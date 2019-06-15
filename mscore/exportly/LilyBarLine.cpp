#include "LilyBarLine.hpp"

#include "libmscore/barline.h"

using namespace Ms;

LilyBarLine::LilyBarLine(const BarLine* barLine) : LilyElement(LILY_BARLINE), _barLine(barLine)
{
}

std::ofstream& LilyBarLine::operator>>(std::ofstream& file) const
{
    switch (_barLine->barLineType())
    {
        case BarLineType::DOUBLE:
            file << "\t\\bar \"||\"" << std::endl;
            break;
        case BarLineType::END:
            file << "\t\\bar \"|.\"" << std::endl;
            break;
        default:
            break;
    }

    return file;
}

void LilyBarLine::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _barLine->accessibleExtraInfo().toStdString() << std::endl;
}
