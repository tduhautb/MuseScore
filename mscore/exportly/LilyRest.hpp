#pragma once

#include "LilyElement.hpp"
#include "libmscore/fraction.h"

namespace Ms
{
class Rest;
class LilyRest final : public LilyElement
{
  private:
    Fraction _fraction;

  public:
    LilyRest(const Rest* rest);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyRest";
    }
    virtual Fraction getFraction() const final;
    void merge(const LilyRest* other);
    bool isFullMeasureRest(const Fraction& timeSig);
};
} // namespace Ms
