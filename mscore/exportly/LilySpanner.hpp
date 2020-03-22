#pragma once

#include "exportly/LilyElement.hpp"

namespace Ms
{
class LilySpanner final : public LilyElement
{
  public:
    typedef enum
    {
        END,
        CRESC,
        DECRESC,
        DIM,
        CRESC_HAIRPIN,
        DECRESC_HAIRPIN,
        TEXT_CRESC,
        TEXT_DECRESC,
        TEXT_DIM,
        SLUR_BEGIN,
        SLUR_END
    } SpannerType;

  private:
    SpannerType _type;

  public:
    LilySpanner(const SpannerType& spannerType);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilySpanner";
    }
    void log(unsigned int indentation) const final;
    bool operator==(const LilySpanner& other);
};

} // namespace Ms
