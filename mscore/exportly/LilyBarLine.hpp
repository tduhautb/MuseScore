#pragma once

#include "LilyElement.hpp"

namespace Ms
{
class BarLine;
class LilyBarLine final : public LilyElement
{
  private:
	  const BarLine* _barLine;

  public:
    LilyBarLine(const BarLine* barLine);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
};
} // namespace Ms
