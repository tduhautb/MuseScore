#pragma once

#include <fstream>
#include <iostream>

#include "LilyElement.hpp"

namespace Ms
{
class LilyMeasure;

class LilyPart final : public LilyElement
{
  private:
    LilyElement* _first;
    LilyMeasure* _currentMeasure;
    unsigned int _nbMeasures;
    std::string _partName;

    void reorganizeClefs();

  public:
    LilyPart(const std::string& partName);
    LilyMeasure* newMeasure();
    virtual std::ofstream& operator>>(std::ofstream& file) const final;

    void reorganize();
    std::string name() const
    {
        return "LilyPart";
    }
};
} // namespace Ms
