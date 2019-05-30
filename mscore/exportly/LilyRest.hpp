#pragma once

#include "LilyElement.hpp"

namespace Ms
{
class Rest;
class LilyRest final : public LilyElement
{
  private:
    unsigned int nbFullMeasures;
    const Rest* _rest;

  public:
    LilyRest(const Rest* rest);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyRest";
    }
    virtual Fraction getFraction() const final;
};
} // namespace Ms
