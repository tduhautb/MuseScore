#pragma once

#include "LilyElement.hpp"

namespace Ms
{
class KeySig;
/*!
 * On prendra la convention que la LilyKey ne se place jamais comme dernier élément de la mesure,
 * toujours en premier élément de la mesure suivante
 * sa sortie se fait sur une ligne où l'évènement est seul
 */
class LilyKey final : public LilyElement
{
  private:
    const KeySig* _keySig;

  public:
    LilyKey(const KeySig* keySig);

    /*! \brief Print key signature change
     *
     * Page layout :
     * - prints one tabulation before the key event
     * - prints a new line at the end
     */
    virtual std::ofstream& operator>>(std::ofstream& file) const;

    bool operator==(const LilyKey& other) const;
    bool operator!=(const LilyKey& other) const;
    std::string name() const
    {
        return "LilyKey";
    }
    void log(unsigned int indentation) const final;
};
} // namespace Ms
