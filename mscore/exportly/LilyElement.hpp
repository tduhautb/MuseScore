#pragma once

#include <fstream>
#include <iostream>

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
};
} // namespace Ms
