#include "LilyArticulation.hpp"

#include "libmscore/articulation.h"
#include "libmscore/sym.h"

using namespace Ms;

LilyArticulation::LilyArticulation(const Articulation* articulation)
    : LilyElement(), _articulation(articulation)
{
}

std::ofstream& LilyArticulation::operator>>(std::ofstream& file) const
{
    switch (_articulation->symId())
    {
        case SymId::articAccentAbove:
        case SymId::articAccentBelow:
            file << "->";
            break;
        case SymId::articStaccatoAbove:
        case SymId::articStaccatoBelow:
            file << "-.";
            break;
        case SymId::articAccentStaccatoAbove:
        case SymId::articAccentStaccatoBelow:
            file << "-> -.";
            break;
        case SymId::articTenutoAbove:
        case SymId::articTenutoBelow:
            file << "--";
            break;
        case SymId::articTenutoAccentAbove:
        case SymId::articTenutoAccentBelow:
            file << "-- ->";
            break;
        case SymId::articTenutoStaccatoAbove:
        case SymId::articTenutoStaccatoBelow:
            file << "-_";
            break;
        case SymId::articMarcatoAbove:
        case SymId::articMarcatoBelow:
            file << "-^";
            break;
        case SymId::articStaccatissimoAbove:
        case SymId::articStaccatissimoBelow:
            file << "-!";
            break;
        case SymId::stringsUpBow:
            file << "\\upbow";
            break;
        case SymId::stringsDownBow:
            file << "\\downbow";
            break;
        case SymId::ornamentMordent:
            file << "\\prall";
            break;
        case SymId::ornamentMordentInverted:
            file << "\\mordent";
            break;
        default:
            break;
    }
    return file;
}
