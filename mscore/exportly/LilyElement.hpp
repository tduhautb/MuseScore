#pragma once

#include <fstream>
#include <iostream>

#include "libmscore/fraction.h"

namespace Ms
{
enum LilyType
{
    LILY_ARTICULATION,
    LILY_BARLINE,
    LILY_CLEF,
    LILY_DYNAMIC,
    LILY_FULLMEASUREREST,
    LILY_KEY,
    LILY_MEASURE,
    LILY_NOTE,
    LILY_PART,
    LILY_REST,
    LILY_SPANNER,
    LILY_TIMESIG,
    LILY_TUPLET
};

class LilyElement
{
  private:
    LilyElement* _prev;
    LilyElement* _next;
    LilyType _type;

  public:
    LilyElement(LilyType type);
    virtual ~LilyElement();
    void setNext(LilyElement* next);
    void setPrev(LilyElement* prev);
    virtual std::ofstream& operator>>(std::ofstream& file) const = 0;
    LilyElement* prev() const;
    LilyElement* next() const;
    virtual std::string name() const = 0;
    virtual Fraction getFraction() const;
    LilyType getType() const;
    virtual void log(unsigned int indentation) const = 0;
};
} // namespace Ms
