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
#include "libmscore/clef.h"
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
    _lastClefType = ClefType::INVALID;
}

bool LilyExporter::exportFile()
{
    printLilyHeaders();

    // iterate over each part
    QList<Part*> parts = _score->parts();
    for (int iPart = 0; iPart < parts.size(); iPart++)
    {
        processPart(parts[iPart]);
    }

    closeFile();

    return true;
}

void LilyExporter::closeFile()
{
    newline();
    _outputFile.close();
}

void LilyExporter::print(const std::string& output)
{
    _outputFile << output;
}

void LilyExporter::newline()
{
    _outputFile << std::endl;
}

void LilyExporter::printLilyHeaders()
{
    // print Lilypond version
    print("\\version \"2.19.82\"");
    newline();

    // define language to use to output the notes
    if (_lang == OutputLanguage::ITALIANO)
    {
        print("\\language \"italiano\"");
        newline();
    }
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

std::string LilyExporter::generatePartName(const Part* part)
{
    std::string instrumentName = part->instrumentName().toStdString();

    // remove eventual spaces
    std::vector<int> toRemove;
    for (unsigned int i = 0; i < instrumentName.length(); i++)
    {
        if (instrumentName[i] == ' ')
            toRemove.push_back(i);
    }

    for (int i = toRemove.size() - 1; i >= 0; i--)
        instrumentName.erase(toRemove[i], 1);

    // append the track to the instrument
    std::string tmpName = instrumentName;
    char added = 'a';

    // check potential conflict with existing instrument
    while (_partNames.find(tmpName) != _partNames.end())
    {
        tmpName = instrumentName + '.' + added;
        added++;
    }

    return tmpName;
}

std::string LilyExporter::getBasePitch(const Part* part, int track)
{
    Note* firstNote = nullptr;

    for (MeasureBase* measure = _score->measures()->first(); measure && !firstNote;
         measure = measure->next())
    {
        if (measure->type() != ElementType::MEASURE)
            continue;

        Measure* mes = dynamic_cast<Measure*>(measure);

        for (Segment* seg = mes->first(); seg && !firstNote; seg = seg->next())
        {
            Element* element = seg->element(track);
            if (!element)
                continue;

            if (element->type() == ElementType::CHORD)
            {
                Chord* chord = dynamic_cast<Chord*>(element);
                firstNote = chord->notes()[0];
            }
        }
    }

    int pitch = firstNote->pitch();
    int rel = std::floor(pitch / 12) - 4;

    std::string relative = noteToLyPitch(firstNote);

    for (int i = rel; i < 0; i++)
        relative += ",";

    for (int i = 0; i < rel; i++)
        relative += "'";

    return relative;
}

std::string LilyExporter::clefName(const ClefType& type) const
{
    std::string name;

    switch (type)
    {
        case ClefType::G:
        case ClefType::G15_MB:
        case ClefType::G8_VB:
        case ClefType::G8_VA:
        case ClefType::G15_MA:
            name = "treble";
            break;
        case ClefType::G8_VB_P:
            name = "treble_(8)";
            break;
        case ClefType::G_1:
            name = "french";
            break;
        case ClefType::C1:
            name = "soprano";
            break;
        case ClefType::C2:
            name = "mezzosoprano";
            break;
        case ClefType::C3:
            name = "alto";
            break;
        case ClefType::C4:
            name = "tenor";
            break;
        case ClefType::C5:
            name = "baritone";
            break;
        case ClefType::F:
        case ClefType::F15_MB:
        case ClefType::F8_VB:
        case ClefType::F_8VA:
        case ClefType::F_15MA:
            name = "bass";
            break;
        case ClefType::F_B:
            name = "varbaritone";
            break;
        case ClefType::F_C:
            name = "subbass";
            break;
        case ClefType::PERC:
        case ClefType::PERC2:
            name = "percussion";
            break;
        default:
            std::cerr << "clef not supported, defaulting with treble clef" << std::endl;
            name = "treble";
            break;
    }

    switch (type)
    {
        case ClefType::G8_VA:
        case ClefType::F_8VA:
            name += "^8";
            break;
        case ClefType::G8_VB:
        case ClefType::F8_VB:
            name += "_8";
            break;
        case ClefType::G15_MA:
        case ClefType::F_15MA:
            name += "^15";
            break;
        case ClefType::G15_MB:
        case ClefType::F15_MB:
            name += "_15";
            break;
        default:
            break;
    }

    return name;
}

/*----------------------------------------------------------
 * Global processing functions
 *----------------------------------------------------------*/

void LilyExporter::processPart(const Part* part)
{
    MeasureBase* measure;
    std::vector<int> usedTracks;
    getUsedTracks(part, usedTracks);

    std::string partName = generatePartName(part);
    _partNames.insert(partName);
    _partToName[part] = partName;

    bool firstClef = true;

    // iterate over the tracks of the part
    for (int track : usedTracks)
    {
        // TODO create function to generate full track name
        std::string trackName = partName + "." + std::to_string(track);
        newline();

        std::string relative = getBasePitch(part, track);

        print(trackName + " = \\relative " + relative + " {");
        newline();

        // iterate over the measures of the score
        for (measure = _score->measures()->first(); measure; measure = measure->next())
        {
            if (measure->type() != ElementType::MEASURE)
                continue;

            Measure* mes = dynamic_cast<Measure*>(measure);

            // suppose for now that the measure is never empty
            print("\t");

            // collect the chords of the measure for the current track
            for (Segment* seg = mes->first(); seg; seg = seg->next())
            {
                Element* element = seg->element(track);
                if (!element)
                    continue;

                processElement(element);

                if (element->type() == ElementType::CLEF && firstClef)
                {
                    newline();
                    print("\t");
                    firstClef = false;
                }
            }

            newline();
        }

        print("}");
    }
}

void LilyExporter::processElement(const Element* element)
{
    switch (element->type())
    {
        case ElementType::CHORD:
            processChord(dynamic_cast<const Chord*>(element));
            break;
        case ElementType::CLEF:
            processClef(dynamic_cast<const Clef*>(element));
        default:
            break;
    }
}

void LilyExporter::processChord(const Chord* chord)
{
    if (chord->notes().size() > 1)
        print("<");

    size_t notesInChord = chord->notes().size();
    size_t currentNote = 1;
    std::string firstLastPitchInChord;
    for (Note* note : chord->notes())
    {
        print(noteToLyPitch(note));
        if (currentNote != notesInChord)
            print(" ");
        currentNote++;
        if (note == chord->notes().front())
            firstLastPitchInChord = _lastPitch;
    }

    if (chord->notes().size() > 1)
        print(">");

    print(lilyDuration(chord));
    print(" ");
    _lastPitch = firstLastPitchInChord;
}

void LilyExporter::processClef(const Clef* clef)
{
    if (clef->clefType() != _lastClefType)
    {
        print("\\clef \"" + clefName(clef->clefType()) + "\" ");
        _lastClefType = clef->clefType();
    }
}

void LilyExporter::getUsedTracks(const Part* part, std::vector<int>& tracks) const
{
    for (int track = part->startTrack(); track != part->endTrack(); track++)
    {
        bool trackUsed = false;
        for (MeasureBase* measure = _score->measures()->first(); measure && !trackUsed;
             measure = measure->next())
        {
            if (measure->type() != ElementType::MEASURE)
                continue;

            Measure* mes = dynamic_cast<Measure*>(measure);

            for (Segment* seg = mes->first(); seg && !trackUsed; seg = seg->next())
            {
                Element* element = seg->element(track);
                if (!element)
                    continue;

                if (element->type() == ElementType::CHORD)
                {
                    tracks.push_back(track);
                    trackUsed = true;
                }
            }
        }
    }
}

} // namespace Ms
