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

bool saveLy(Score *score, const QString &name)
{
    LilyExporter exporter(score, name);
    return exporter.exportFile();
}

LilyExporter::LilyExporter(Score *score, const QString &filename)
    : _score(score), _filename(filename), _lang(ITALIANO)
{
}

bool LilyExporter::exportFile()
{
    MeasureBase *measure;
    // on itère sur chaque part
    QList<Part *> parts = _score->parts();
    for (int iPart = 0; iPart < parts.size(); iPart++)
    {
        Part *part = parts[iPart];
        for (int track = part->startTrack(); track < part->endTrack(); track++)
        {
            bool trackModified = false;
            for (measure = _score->measures()->first(); measure; measure = measure->next())
            {
                // quick hack to handle empty segments to spare terminal space
                bool modified = false;
                if (measure->type() != ElementType::MEASURE)
                    continue;

                Measure *mes = dynamic_cast<Measure *>(measure);

                std::vector<Chord *> measureChords;

                for (Segment *seg = mes->first(); seg; seg = seg->next())
                {
                    Element *element = seg->element(track);
                    if (!element)
                        continue;

                    if (element->type() == ElementType::CHORD)
                    {
                        modified = true;
                        trackModified = true;
                        Chord *chord = dynamic_cast<Chord *>(element);
                        measureChords.push_back(chord);
                    }
                }

                for (Chord *chord : measureChords)
                {
                    if (chord->notes().size() > 1)
                        std::cout << "<";

                    size_t notesInChord = chord->notes().size();
                    size_t currentNote = 1;
                    for (Note *note : chord->notes())
                    {
                        std::cout << noteToLyPitch(note);
                        if (currentNote != notesInChord)
                            std::cout << " ";
                        currentNote++;
                    }

                    if (chord->notes().size() > 1)
                        std::cout << ">";

                    if (chord != measureChords.back())
                        std::cout << " ";
                }

                if (modified)
                    std::cout << std::endl;
            }

            if (trackModified)
                std::cout << std::endl;
        }
    }

    return true;
}

std::string LilyExporter::noteToLyPitch(const Note *note)
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

    std::string lyPitch = _pitchToNote[_lang][pitch] + _accidentalName[_lang][accName];

    return lyPitch;
}
} // namespace Ms
