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
class Chord;
class DurationElement;
class Part;

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
    Score* _score;             /*!< score to export */
    OutputLanguage _lang;      /*!< output language for notes notation */
    std::ofstream _outputFile; /*!< output file */
    std::string _lastPitch;    /*!< last pitch printed to the file */

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
    std::string noteToLyPitch(const Note* note);

    /*! \brief Retrieve the lilypond duration of the given element
     *
     * Doesn't consider the eventual tuplet surrounding the element. Handle
     * dotted durations.
     *
     * \param[in] the DurationElement to process
     * \return a string object corresponding to the duration of the given element
     */
    std::string lilyDuration(const DurationElement* element);

    /*! \brief Compute the relative height with the previous note
     *
     * Compute the relative height between the previous note and the note given
     * in parameter.
     *
     * \param[in] note the current pitch to process;
     * \return a string object with the height modifier to add to the note name
     */
    std::string relativeHeight(const std::string& currentPitch);


	/*----------------------------------------------------------
	 *  Global processing
	 *----------------------------------------------------------*/

	/*! \brief Process part
	 *
	 * Process the given part to extract the music.
	 *
	 * \param[in] part the Part to process
	 */
	void processPart(const Part* part);

	/*! \brief Return the tracks containing at least one note
	 *
	 * Iterates through the tracks of the given part to select only those containing
	 * at least a chord.
	 *
	 * \param[in] part the Part to process
	 * \param[out] tracks the list of used tracks
	 */
	void getUsedTracks(const Part* part, std::vector<int>& tracks) const;

    /*! \brief Print Lilypond headers
     *
     * Prints the Lilypond version and language headers at the top of the file
     */
    void printLilyHeaders();

    /*! \brief Close the output file at the end of the export process */
    void closeFile();

  public:
    /*! \brief Constructor : initialize members and create output file */
    LilyExporter(Score* score, const QString& filename);

    /*! \brief Entry point, export the score to a Lilypond file */
    bool exportFile();
};
} // namespace Ms
