#pragma once

#include "LilyElement.hpp"

#include "libmscore/chord.h"

namespace Ms
{
class LilyNote final : public LilyElement
{
  public:
    /*!< \brief conversion table from pitch (0/A-6/G) to note name */
    static const std::string _pitchToNote[2][7];

    /*!< \brief conversion table from accidental to corresponding suffix */
    static const std::string _accidentalName[2][5];

    /*! \brief the type of the accidental alteration with respect to the order of the
     * \p _accidentalName array order
     */
    typedef enum
    {
        LYNATURAL = 0, /*!< natural note */
        LYSHARP,       /*!< sharp note */
        LYFLAT,        /*!< flat note */
        LYSSHARP,      /*!< double sharp note */
        LYFFLAT        /*!< double flat note */
    } LyAccidentalName;

  private:
	const Chord* _chord;

	std::string noteToLy(const Note* note) const;

  public:
    LilyNote(const Chord* chord);
    virtual std::ofstream& operator>>(std::ofstream& file) const final;

    static std::string tpcToPitch(unsigned int tpc);
};
} // namespace Ms
