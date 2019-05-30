#pragma once

#include "LilyElement.hpp"

#include "libmscore/fraction.h"

namespace Ms
{
class TimeSig;
class LilyTimeSig final : public LilyElement
{
  private:
    const TimeSig* _timeSig;

  public:
    LilyTimeSig(const TimeSig* timeSig);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;

    bool operator==(const LilyTimeSig& other) const;
    bool operator!=(const LilyTimeSig& other) const;
    std::string name() const
    {
        return "LilyTimeSig";
    }

    virtual Fraction getFraction() const final;
};
} // namespace Ms
