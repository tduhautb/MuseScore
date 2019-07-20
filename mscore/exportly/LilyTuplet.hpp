#pragma once

#include "exportly/LilyElement.hpp"

namespace Ms
{
class Tuplet;

class LilyTuplet final : public LilyElement
{
  private:
    std::vector<LilyElement*> _elements;
    Fraction _ratio;
    Fraction _cumulatedFraction;

  public:
    LilyTuplet(const Tuplet* tuplet);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    std::string name() const
    {
        return "LilyTuplet";
    }

    /*! \brief Add an element to the tuple */
    void addElement(LilyElement* element);

    /*! \brief Returns the apparent fraction of the tuple */
    virtual Fraction getFraction() const final;

    void log(unsigned int indentation) const final;
};
} // namespace Ms
