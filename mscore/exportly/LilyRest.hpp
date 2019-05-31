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
    bool _fullMeasureRest;

  public:
    LilyRest(const Rest* rest);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyRest";
    }
    virtual Fraction getFraction() const final;
    void merge(const LilyRest* other);
    void checkFullMeasureRest(const Fraction& timeSig);
    bool isFullMeasure() const;
};
} // namespace Ms
