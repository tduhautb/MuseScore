#pragma once

#include "LilyElement.hpp"

namespace Ms
{
class Articulation;

class LilyArticulation final : public LilyElement
{
  private:
    const Articulation* _articulation;

  public:
    LilyArticulation(const Articulation* articulation);
    std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyArticulation";
    }

    void log(unsigned int indentation) const final;
};
} // namespace Ms
