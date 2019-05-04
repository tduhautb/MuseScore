#pragma once

#include "LilyElement.hpp"

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
};
} // namespace Ms
