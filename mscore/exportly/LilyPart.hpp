#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "LilyElement.hpp"

namespace Ms
{
class LilyMeasure;

class LilyPart final : public LilyElement
{
  private:
    std::vector<unsigned int> _tracks;
    std::map<unsigned int, LilyElement*> _first;
    std::map<unsigned int, LilyMeasure*> _currentMeasure;
    std::map<unsigned int, unsigned int> _nbMeasures;
    std::string _partName;

    void reorganizeClefs();

  public:
    LilyPart(const std::string& partName, std::vector<unsigned int> tracks);
    LilyMeasure* newMeasure(unsigned int track);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;

    void reorganize();

    std::string name() const
    {
        return "LilyPart";
    }

    /*----------------------------------------------------------
     *  Simplify the score at the given element with respect to
     *  the current one
     *----------------------------------------------------------*/
    template <class T> bool simplify(const T* current, T const** reference, unsigned int track)
    {
        // if the reference is empty (first time the current element type is seen) or if the
        // elements are different, update the referecne accordingly
        if (!*reference || *current != **reference)
        {
            *reference = current;
            return false;
        }
        else
        {
            // this element is a duplicate, disconnect it from the part
            // this function doesn't perform the deletion !
            if (current == _first[track])
                _first[track] = current->next();

            if (current->prev())
                current->prev()->setNext(current->next());

            if (current->next())
                current->next()->setPrev(current->prev());

            // the element can be deleted
            return true;
        }
    }

    void log(unsigned int indentation) const final;
};
} // namespace Ms
