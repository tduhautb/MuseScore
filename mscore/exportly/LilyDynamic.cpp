#include "LilyDynamic.hpp"

#include "libmscore/dynamic.h"

using namespace Ms;

LilyDynamic::LilyDynamic(const Dynamic* dynamic) : LilyElement(LILY_DYNAMIC), _dynamic(dynamic)
{
}

std::ofstream& LilyDynamic::operator>>(std::ofstream& file) const
{
    switch (_dynamic->dynamicType())
    {
        case Dynamic::Type::P:
            file << "\\p";
            break;
        case Dynamic::Type::PP:
            file << "\\pp";
            break;
        case Dynamic::Type::PPP:
            file << "\\ppp";
            break;
        case Dynamic::Type::PPPP:
            file << "\\pppp";
            break;
        case Dynamic::Type::PPPPP:
        case Dynamic::Type::PPPPPP:
            file << "\\ppppp";
            break;
        case Dynamic::Type::F:
            file << "\\f";
            break;
        case Dynamic::Type::FF:
            file << "\\ff";
            break;
        case Dynamic::Type::FFF:
            file << "\\fff";
            break;
        case Dynamic::Type::FFFF:
            file << "\\ffff";
            break;
        case Dynamic::Type::FFFFF:
        case Dynamic::Type::FFFFFF:
            file << "\\fffff";
            break;
        case Dynamic::Type::MF:
            file << "\\mf";
            break;
        case Dynamic::Type::MP:
            file << "\\mp";
            break;
        case Dynamic::Type::FP:
            file << "\\fp";
            break;
        case Dynamic::Type::FZ:
        case Dynamic::Type::SFZ:
        case Dynamic::Type::SFFZ:
        case Dynamic::Type::SFP:
        case Dynamic::Type::SFPP:
            file << "\\sfz";
            break;
        case Dynamic::Type::RFZ:
            file << "\\rfz";
            break;
        case Dynamic::Type::SF:
            file << "\\sf";
            break;
        case Dynamic::Type::SFF:
            file << "\\sff";
            break;
        default:
            break;
    }

    return file;
}

void LilyDynamic::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _dynamic->accessibleExtraInfo().toStdString() << std::endl;
}
