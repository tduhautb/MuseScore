//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//  $Id: exportly.cpp 5510 2012-03-30 14:51:09Z wschweer $
//
//  Copyright (C) 2007 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#pragma once

#include <fstream>
#include <iostream>

#include <QtCore/QString>

namespace Ms
{
class Score;
class Note;

/*! \brief A LilyPond exporter class
 *
 * This class handles the export from MuseScore internal structure to a LilyPond file.
 * For more information about LilyPond, visit http://lilypond.org/
 */

class LilyExporter
{
    /*! \brief the output language for the notation in the Lilypond file */
    typedef enum
    {
        ENGLISH = 0, /*!< use the letter notation A-G */
        ITALIANO     /*!< use the italian notation do - si */
    } OutputLanguage;

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

  public:
    /*!< \brief conversion table from pitch (0/A-6/G) to note name */
    static const std::string _pitchToNote[2][7];

    /*!< \brief conversion table from accidental to corresponding suffix */
    static const std::string _accidentalName[2][5];

  private:
    Score *_score;             /*!< score to export */
    OutputLanguage _lang;      /*!< output language for notes notation */
    std::ofstream _outputFile; /*!< output file */

    /*! \brief Convert the given Note into a Lilypond pitch
     *
     * Convert the note into a Lilypond pitch without consideration of the relative height
     * of the note.
     * Use the Ms::tpc2name function to get the name of the pitch in standard notation and
     * convert to LilyPond pitch.
     *
     * \param[in] note the Note object to convert to a Lilypond pitch
     * \return a string object corresponding to the Lilypond pitch of the given note
     */
    std::string noteToLyPitch(const Note *note);

    /*! \brief Close the output file at the end of the export process */
    void closeFile();

  public:
    /*! \brief Constructor : initialize members and create output file */
    LilyExporter(Score *score, const QString &filename);

    /*! \brief Entry point, export the score to a Lilypond file */
    bool exportFile();
};
} // namespace Ms
