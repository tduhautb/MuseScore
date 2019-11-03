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

// forward declaration of the Qt Classes
class QPushButton;
class QVBoxLayout;
class QRadioButton;
class QButtonGroup;

namespace Ms
{
class LilyMeasure;
class LilyArticulation;
class LilyClef;
class LilyDynamic;
class LilyElement;
class LilyKey;
class LilyMeasure;
class LilyNote;
class LilyPart;
class LilySpanner;
class LilyTimeSig;

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
class Dynamic;

/*! \brief A LilyPond exporter class
 *
 * This class handles the export from MuseScore internal structure to a LilyPond file.
 * For more information about LilyPond, visit http://lilypond.org/
 */

class LilyExporter
{
  public:
    /*! \brief the output language for the notation in the Lilypond file */
    typedef enum
    {
        ENGLISH = 0, /*!< use the letter notation A-G */
        ITALIANO     /*!< use the italian notation do - si */
    } OutputLanguage;

    struct LilyExportOptions {
        OutputLanguage _lang; /*!< output language for notes notation */
        bool _singleFile;     /*!< export all parts in the same file */
    };

    /*! \brief Retrieve the lilypond duration of the given element
     *
     * Doesn't consider the eventual tuplet surrounding the element. Handle
     * dotted durations.
     *
     * \param[in] the DurationElement to process
     * \return a string object corresponding to the duration of the given element
     */
    static std::string lilyDuration(Fraction frac);

    static LilyExporter* getInstance();

    static LilyExporter* createInstance(Score* score, const QString& filename, QMainWindow* mscore);

    static void freeInstance();

  private:
    static LilyExporter* _instance; /*!< static instance element */
    LilyExportOptions _options;     /*!< export options */
    Score* _score;                  /*!< score to export */
    const QString& _filename;       /*!< name of the output file */
    QMainWindow* _mscore;           /*!< pointer to the Qt main window (used to display dialogs) */
    std::ofstream _outputFile;      /*!< output file */
    std::set<std::string> _partNames; /*!< lilypond name associated to each part */
    std::map<const Part*, std::string>
        _partToName; /*!< map between MuseScore part and lilypond name*/
    std::map<const Part*, std::vector<const Staff*>>
        _partToStaves; /*!< mapping from a part to the corresponding staves*/
    std::map<const Staff*, std::vector<unsigned int>>
        _staffToTracks; /*!< mapping from a staff to the corresponding tracks*/

    /*! \brief Generate a name for the current part
     *
     * Generate a unique name for the part to identify the music exported in the
     * lilypond file.
     *
     * \param[in] part the current part to process
     */
    std::string generatePartName(const Part* part);

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
    LilyElement* processElement(const Element* element);

    /*! \brief Return the tracks containing at least one note
     *
     * Iterates through the tracks of the given part to select only those containing
     * at least a chord.
     *
     * \param[in] part the Part to process
     * \param[out] tracks the list of used tracks
     */
    void getUsedTracks(const Part* part, std::vector<unsigned int>& tracks);

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

    void printTracks(const std::string partName, const std::vector<unsigned int>& tracks);

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

    /*! \brief Constructor : initialize members and create output file */
    LilyExporter(Score* score, const QString& filename, QMainWindow* mscore);

    /*! \brief Show the options dialog window and update the export options of the instance */
    void requestOptions();

  public:
    /*! \brief Entry point, export the score to a Lilypond file */
    bool exportFile();

    OutputLanguage getLang() const;

    void checkSpanner(const ChordRest* chordRest, bool begin);
};

} // namespace Ms
