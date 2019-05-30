#pragma once

#include "exportly/LilyClef.hpp"
#include "exportly/LilyElement.hpp"
#include "exportly/LilyKey.hpp"

#include "libmscore/fraction.h"

namespace Ms
{
class LilyTimeSig;

class LilyMeasure final : public LilyElement
{
  private:
    LilyElement* _first;
    LilyElement* _current;
    bool _anacrousis;
    Fraction _anacrousisFraction;
    unsigned int _measureNum;

    // disconnect an element in the measure
    void disconnectElement(const LilyElement* element);

    // print the anacrousis
    std::string printAnacrousis() const;

  public:
    LilyMeasure(unsigned int num);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;
    void addElement(LilyElement* element);
    template <class T> void simplify(T const** currentElement)
    {
        for (LilyElement* current = _first; current; current = current->next())
        {
            T* tmpElement = dynamic_cast<T*>(current);

            if (!tmpElement)
                continue;

            if (!*currentElement || (*tmpElement != **currentElement))
            {
                // the clef is a new clef, keep it
                *currentElement = tmpElement;
            }
            else
            {
                // the clef is already defined, drop the element
                disconnectElement(tmpElement);
                delete tmpElement;
            }
        }
    }
    void moveKeyToNextMeasure();
    std::string name() const
    {
        return "LilyMeasure";
    }

    // check is the measure is complete or not
    void checkAnacrousis(const LilyTimeSig* timeSig);
};
} // namespace Ms
