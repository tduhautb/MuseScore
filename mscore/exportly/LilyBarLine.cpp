#include "LilyBarLine.hpp"

#include "libmscore/barline.h"

using namespace Ms;

LilyBarLine::LilyBarLine(const BarLine* barLine) : LilyElement(), _barLine(barLine)
{
}

std::ofstream& LilyBarLine::operator>>(std::ofstream& file) const
{
    switch (_barLine->barLineType())
    {
        case BarLineType::DOUBLE:
            file << "\\bar \"||\"";
            break;
        case BarLineType::END:
            file << "\\bar \"|.\"";
            break;
        default:
            break;
    }

    return file;
}
