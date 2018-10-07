//=============================================================================
//  MusE Score
//  Linux Music Score Editor
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

#include "exportly.hpp"
#include <exception>

#include "libmscore/accidental.h"
#include "libmscore/chord.h"
#include "libmscore/fraction.h"
#include "libmscore/measure.h"
#include "libmscore/measurebase.h"
#include "libmscore/note.h"
#include "libmscore/part.h"
#include "libmscore/score.h"
#include "libmscore/scoreElement.h"
#include "libmscore/tuplet.h"
#include "libmscore/types.h"
#include "musescore.h"

#define COUT(txt) std::cout << txt << std::endl

const std::string Ms::LilyExporter::_pitchToNote[2][7] = {
    {"a", "b", "c", "d", "e", "f", "g"}, {"la", "si", "do", "re", "mi", "fa", "sol"}};
const std::string Ms::LilyExporter::_accidentalName[2][5] = {{"", "is", "es", "isis", "eses"},
                                                             {"", "d", "b", "dd", "bb"}};

namespace Ms
{

bool saveLy(Score* score, const QString& name)
{
    LilyExporter exporter(score, name);
    try
    {
        return exporter.exportFile();
    }
    catch (const std::exception& e)
    {
        COUT(e.what());
    }
    catch (...)
    {
        COUT("exception caught");
    }

    return false;
}

LilyExporter::LilyExporter(Score* score, const QString& filename) : _score(score), _lang(ITALIANO)
{
    // truncate existing file
    _outputFile.open(filename.toStdString(), ios::trunc);
}

bool LilyExporter::exportFile()
{
    printLilyHeaders();

    MeasureBase* measure;
    // iterate over each part
    QList<Part*> parts = _score->parts();
    for (int iPart = 0; iPart < parts.size(); iPart++)
    {
        Part* part = parts[iPart];
        for (int track = part->startTrack(); track < part->endTrack(); track++)
        {
            bool trackModified = false;
            for (measure = _score->measures()->first(); measure; measure = measure->next())
            {
                // quick hack to handle empty segments to spare terminal space
                bool modified = false;
                if (measure->type() != ElementType::MEASURE)
                    continue;

                Measure* mes = dynamic_cast<Measure*>(measure);

                std::vector<Chord*> measureChords;

                for (Segment* seg = mes->first(); seg; seg = seg->next())
                {
                    Element* element = seg->element(track);
                    if (!element)
                        continue;

                    if (element->type() == ElementType::CHORD)
                    {
                        modified = true;
                        trackModified = true;
                        Chord* chord = dynamic_cast<Chord*>(element);
                        measureChords.push_back(chord);
                    }
                }

                for (Chord* chord : measureChords)
                {
                    if (chord->notes().size() > 1)
                        _outputFile << "<";

                    size_t notesInChord = chord->notes().size();
                    size_t currentNote = 1;
                    std::string firstLastPitchInChord;
                    for (Note* note : chord->notes())
                    {
                        _outputFile << noteToLyPitch(note);
                        if (currentNote != notesInChord)
                            _outputFile << " ";
                        currentNote++;
                        if (note == chord->notes().front())
                            firstLastPitchInChord = _lastPitch;
                    }

                    _lastPitch = firstLastPitchInChord;

                    if (chord->notes().size() > 1)
                        _outputFile << ">";

                    _outputFile << lilyDuration(chord);

                    if (chord != measureChords.back())
                        _outputFile << " ";
                }

                if (modified)
                    _outputFile << std::endl;
            }

            if (trackModified)
                _outputFile << std::endl;
        }
    }

    closeFile();

    return true;
}

void LilyExporter::closeFile()
{
    _outputFile.close();
}

void LilyExporter::printLilyHeaders()
{
    // print Lilypond version
    _outputFile << "\\version \"2.19.82\"" << std::endl;

    // define language to use to output the notes
    if (_lang == OutputLanguage::ITALIANO)
        _outputFile << "\\language \"italiano\"" << std::endl;

    _outputFile << std::endl;
}

std::string LilyExporter::noteToLyPitch(const Note* note)
{
    std::string pitchName =
        tpc2name(note->tpc(), NoteSpellingType::STANDARD, NoteCaseType::CAPITAL, false)
            .toStdString();
    int pitch = pitchName[0] - 'A';
    LyAccidentalName accName = LYNATURAL;

    if (pitchName.length() > 1)
    {
        switch (pitchName[1])
        {
            case '#':
                if (pitchName.length() == 3)
                    accName = LYSSHARP;
                else
                    accName = LYSHARP;
                break;
            case 'b':
                if (pitchName.length() == 3)
                    accName = LYFFLAT;
                else
                    accName = LYFLAT;
                break;
            default:
                std::cerr << "UNKNOWN ALTERATION";
                break;
        }
    }

    // add the octave value
    pitchName += std::to_string((note->epitch() / 12) - 1);

    std::string lyPitch = _pitchToNote[_lang][pitch];
    lyPitch += _accidentalName[_lang][accName];
    lyPitch += relativeHeight(pitchName);

    return lyPitch;
}

std::string LilyExporter::lilyDuration(const DurationElement* element)
{
    Fraction frac = element->duration().reduced();
    int base = 0;
    unsigned int nbDots = 0;

    switch (frac.numerator())
    {
        case 1:
            base = frac.denominator();
            break;
        case 3:
            base = frac.denominator() / 2;
            nbDots = 1;
            break;
        case 7:
            base = frac.denominator() / 4;
            nbDots = 2;
            break;
        case 15:
            base = frac.denominator() / 8;
            nbDots = 3;
            break;
        case 31:
            base = frac.denominator() / 16;
            nbDots = 4;
            break;
        default:
            std::cerr << "Unhandled time fraction " << frac.numerator() << "/" << frac.denominator()
                      << std::endl;
            std::cerr << element->accessibleInfo().toStdString() << std::endl;
            throw(-2);
            break;
    }

    std::string duration = std::to_string(base);
    for (unsigned int i = 0; i < nbDots; i++)
    {
        duration += ".";
    }

    return duration;
}

std::string LilyExporter::relativeHeight(const std::string& pitch)
{
    std::string relative("");

    if (_lastPitch.length() > 0)
    {
        // compute the interval between the previous and the current pitch
        int prevPitch = (_lastPitch[0] - 'C'); // C = 0, D = 1, ...
        if (prevPitch < 0)
            prevPitch += 7; // to ensure A = 5, B = 6

        // adjust with the octave
        prevPitch = prevPitch + 7 * (_lastPitch.back() - '0');

        int currentPitch = (pitch[0] - 'C');
        if (currentPitch < 0)
            currentPitch += 7;
        currentPitch = currentPitch + 7 * (pitch[pitch.size() - 1] - '0');
        int diff = currentPitch - prevPitch;

        while (diff >= 4)
        {
            relative += "'";
            diff -= 7;
        }

        while (diff <= -4)
        {
            relative += ",";
            diff += 7;
        }
    }

    _lastPitch = pitch;

    return relative;
}

} // namespace Ms
