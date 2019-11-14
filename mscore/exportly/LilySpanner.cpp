#include "LilySpanner.hpp"

#include "libmscore/spanner.h"

using namespace Ms;

LilySpanner::LilySpanner(const SpannerType& spannerType)
    : LilyElement(LILY_SPANNER), _type(spannerType)
{
}

std::ofstream& LilySpanner::operator>>(std::ofstream& file) const
{
    switch (_type)
    {
        case END:
            file << "\\!";
            break;
        case CRESC:
            file << "\\cresc";
            break;
        case DECRESC:
            file << "\\decresc";
            break;
        case DIM:
            file << "\\dim";
            break;
        case CRESC_HAIRPIN:
            file << "\\<";
            break;
        case DECRESC_HAIRPIN:
            file << "\\>";
            break;
        case TEXT_CRESC:
            file << "_{\\markup{\\italic cresc.}}";
            break;
        case TEXT_DECRESC:
            file << "_{\\markup{\\italic decresc.}}";
            break;
        case TEXT_DIM:
            file << "_{\\markup{\\italic dim.}}";
            break;
        case SLUR_BEGIN:
            file << "(";
            break;
        case SLUR_END:
            file << ")";
            break;
    }
    return file;
}

void LilySpanner::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " TODO" << std::endl;
}
