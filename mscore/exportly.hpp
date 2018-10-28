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

#include <map>
#include <set>

#include <fstream>
#include <iostream>

#include "libmscore/clef.h"
#include "libmscore/keysig.h"

#include <QtCore/QString>

namespace Ms
{
class Articulation;
class Score;
class Element;
class Note;
class Chord;
class DurationElement;
class Part;
class TimeSig;
class Rest;
class BarLine;

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
    Score* _score;                    /*!< score to export */
    OutputLanguage _lang;             /*!< output language for notes notation */
    std::ofstream _outputFile;        /*!< output file */
    std::string _lastPitch;           /*!< last pitch printed to the file */
    std::set<std::string> _partNames; /*!< lilypond name associated to each part */
    std::map<const Part*, std::string>
        _partToName;             /*!< map between MuseScore part and lilypond name*/
    ClefType _lastClefType;      /*!< last clef type printed for the part */
    const KeySig* _lastKey;      /*!< last key printed for the part */
    const TimeSig* _lastTimeSig; /*!< last time sig printed for the part */

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

    /*! \brief Generate a name for the current part
     *
     * Generate a unique name for the part to identify the music exported in the
     * lilypond file.
     *
     * \param[in] part the current part to process
     */
    std::string generatePartName(const Part* part);

    /*! \brief Get the base pitch for the current part and track
     *
     * Retrieve the first note of the current part to set it as the reference for
     * the height of the next notes.
     * This function is used to generate the \relative command at the beginning of
     * the music.
     *
     * The midi pitch 48 corresponds to the default height of C in Lilypond. Therefore,
     * the height 4 (48/12) is considered as the 0-level in Lilypond to define the relative
     * height. If the octava of the first note is higher, some ' modifiers will be added.
     * If it is lower, some , modifiers will be added.
     *
     * \param[in] part the current part to process
     * \param[in] track the current track
     */
    std::string getBasePitch(const Part* part, int track);

    /*! \brief Get the LilyPond name of the given clef
     *
     * Default clefs are currently supporter : standard G, F, and C. Tabs clefs
     * are not handled by this function.
     *
     * \param[in] clef the clef to process
     * \return the LilyPond name of the given clef.
     */
    std::string clefName(const ClefType& clefType) const;

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

    /*! \brief Process an element
     *
     * Process the given element according to its type.
     *
     * \param[in] element the Element to process
     */
    void processElement(const Element* element);

    /*! \brief Process a chord
     *
     * Process the given chord to extract the notes
     *
     * \param[in] chord the Chord to process
     */
    void processChord(const Chord* chord);

    /*! \brief Process a clef
     *
     * Process the given clef : compare its type with the previous clef printed.
     * If the type is the same, don't print the clef (beginning of the line...).
     *
     * \param[in] clef the Clef to process
     */
    void processClef(const Clef* clef);

    /*! \brief Process a key signature
     *
     * Process the given key signature : compare it with the previous signature
     * printed. If the type is the same, don't print the signature again.
     *
     * \param[in] keySig the KeySig to process
     */
    void processKeySig(const KeySig* keySig);

    /*! \brief Process a time signature
     *
     * Process the given time signature : compare it with the previous signature
     * printed. If the time signature is the same, don't print the sig again.
     *
     * \param[in] timeSig the TimeSig to process
     */
    void processTimeSig(const TimeSig* timeSig);

    /*! \brief Process a rest
     *
     * Process the given rest : print it to the file as a partial rest or a measure rest.
     * Currently does not support multi-measures rests.
     *
     * \param[in] rest the Rest to process
     *
     * \todo handle multi measures rests
     */
    void processRest(const Rest* rest);

    /*! \brief Process bar lines
     *
     * Process the given bar line.
     *
     * Currently does not support rehearsal bars.
     *
     * \param[in] barLine the barLine to process
     */
    void processBarLine(const BarLine* barLine);

    /*! \brief Process articulations
     *
     * Process the gien articulation.
     *
     * Currently supports :
     * - accent
     * - staccato
     * - marcato
     * - staccatissimo
     * - tenuto
     * - up bow (strings)
     * - down bow (strings)
     * - mordent / inverted mordent
     *
     * \param[in] articulation the Articulation to process
     */
    void processArticulation(const Articulation* articulation);

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

    /*! \brief Print the staff definition for the given part
     *
     * Print the Staff commands for the given part. Currently supports only one
     * track per part.
     *
     * \param[in] part the Part to process
     */
    void printPartStaff(const Part* part);

    /*----------------------------------------------------------
     * Output file related functions
     *----------------------------------------------------------*/

    /*! \brief Print the string to the file
     *
     * \param output the string to print to the file
     */
    void print(const std::string& output);

    /*! \brief Print a new line to the file */
    void newline();

    /*! \brief Close the output file at the end of the export process */
    void closeFile();

  public:
    /*! \brief Constructor : initialize members and create output file */
    LilyExporter(Score* score, const QString& filename);

    /*! \brief Entry point, export the score to a Lilypond file */
    bool exportFile();
};
} // namespace Ms
