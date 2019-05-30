#pragma once

#include <fstream>
#include <iostream>

#include "libmscore/fraction.h"

namespace Ms
{
class LilyElement
{
  private:
    LilyElement* _prev;
    LilyElement* _next;

  public:
    LilyElement();
    void setNext(LilyElement* next);
    void setPrev(LilyElement* prev);
    virtual std::ofstream& operator>>(std::ofstream& file) const = 0;
    LilyElement* prev() const;
    LilyElement* next() const;
    virtual std::string name() const = 0;
    virtual Fraction getFraction() const;
};
} // namespace Ms
