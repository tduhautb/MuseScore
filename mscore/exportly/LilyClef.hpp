#pragma once

#include "LilyElement.hpp"

namespace Ms
{
class Clef;
/*! on peut changer la clé en milieu de mesure
 * La clé est exportée sans espace en fin de ligne
 */
class LilyClef final : public LilyElement
{
  private:
    const Clef* _clef;

    std::string clefName() const;

  public:
    LilyClef(const Clef* clef);
    virtual std::ofstream& operator>>(std::ofstream& file) const;
    bool operator==(const LilyClef& other) const;
    bool operator!=(const LilyClef& other) const;
    std::string name() const
    {
        return "LilyClef";
    }
};
} // namespace Ms
