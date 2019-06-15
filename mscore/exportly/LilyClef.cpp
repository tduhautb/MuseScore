#include "LilyClef.hpp"

#include "libmscore/clef.h"

using namespace Ms;

LilyClef::LilyClef(const Clef* clef) : LilyElement(LILY_CLEF), _clef(clef)
{
}

std::ofstream& LilyClef::operator>>(std::ofstream& file) const
{
    file << "\\clef \"" + clefName() + "\" ";
    return file;
}

std::string LilyClef::clefName() const
{
    std::string name;

    switch (_clef->clefType())
    {
        case ClefType::G:
        case ClefType::G15_MB:
        case ClefType::G8_VB:
        case ClefType::G8_VA:
        case ClefType::G15_MA:
            name = "treble";
            break;
        case ClefType::G8_VB_P:
            name = "treble_(8)";
            break;
        case ClefType::G_1:
            name = "french";
            break;
        case ClefType::C1:
            name = "soprano";
            break;
        case ClefType::C2:
            name = "mezzosoprano";
            break;
        case ClefType::C3:
            name = "alto";
            break;
        case ClefType::C4:
            name = "tenor";
            break;
        case ClefType::C5:
            name = "baritone";
            break;
        case ClefType::F:
        case ClefType::F15_MB:
        case ClefType::F8_VB:
        case ClefType::F_8VA:
        case ClefType::F_15MA:
            name = "bass";
            break;
        case ClefType::F_B:
            name = "varbaritone";
            break;
        case ClefType::F_C:
            name = "subbass";
            break;
        case ClefType::PERC:
        case ClefType::PERC2:
            name = "percussion";
            break;
        default:
            std::cerr << "clef not supported, defaulting with treble clef" << std::endl;
            name = "treble";
            break;
    }

    switch (_clef->clefType())
    {
        case ClefType::G8_VA:
        case ClefType::F_8VA:
            name += "^8";
            break;
        case ClefType::G8_VB:
        case ClefType::F8_VB:
            name += "_8";
            break;
        case ClefType::G15_MA:
        case ClefType::F_15MA:
            name += "^15";
            break;
        case ClefType::G15_MB:
        case ClefType::F15_MB:
            name += "_15";
            break;
        default:
            break;
    }

    return name;
}

bool LilyClef::operator==(const LilyClef& other) const
{
    return _clef->clefType() == other._clef->clefType();
}

bool LilyClef::operator!=(const LilyClef& other) const
{
    return !(*this == other);
}

void LilyClef::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _clef->accessibleInfo().toStdString() << std::endl;
}
