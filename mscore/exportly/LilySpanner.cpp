#include "LilySpanner.hpp"

using namespace Ms;

LilySpanner::LilySpanner(const SpannerType& spannerType) : LilyElement(), _type(spannerType)
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
    }
    return file;
}
