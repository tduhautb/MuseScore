#pragma once

#include "exportly/LilyElement.hpp"

namespace Ms
{
class LilyFullMeasureRest final : public LilyElement
{
  private:
    unsigned int _nbFullMeasures;
    unsigned int _numFirstMeasure;
    Fraction _fraction;

  public:
    LilyFullMeasureRest(const Fraction& fraction, unsigned int measureNum);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyFullMeasureRest";
    }
    void addFullMeasure(unsigned int nb = 1);
    void log(unsigned int indentation) const final;
};
} // namespace Ms
