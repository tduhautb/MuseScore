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

const std::string Ms::LilyExporter::_pitchToNote[2][12] = {
    {"c", " ", "d", " ", "e", "f", " ", "g", " ", "a", " ", "b"},
    {"do", " ", "re", " ", "mi", "fa", " ", "sol", " ", "la", " ", "si"}};
const std::string Ms::LilyExporter::_accidentalName[2][5] = {{"", "is", "es", "iis", "ees"},
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
    COUT(parts.size() << " parts");
    for (int iPart = 0; iPart < parts.size(); iPart++)
    {
        Part *part = parts[iPart];
        COUT("nouvelle partie");
        for (int track = part->startTrack(); track < part->endTrack(); track++)
        {
            COUT("track " << track);
            unsigned int imes = 0;
            for (measure = _score->measures()->first(); measure; measure = measure->next())
            {
                if (measure->type() != ElementType::MEASURE)
                    continue;

                Measure *mes = dynamic_cast<Measure *>(measure);
                COUT("\tmesure " << std::to_string(imes++));

                unsigned int iseg = 0;
                for (Segment *seg = mes->first(); seg; seg = seg->next(), iseg++)
                {
                    Element *element = seg->element(track);
                    if (!element)
                        continue;
                    COUT("\t\tsegment " << std::to_string(iseg));

                    if (element->type() == ElementType::CHORD)
                    {
                        Chord *chord = dynamic_cast<Chord *>(element);
                        Fraction duration = chord->actualFraction();
                        COUT("\t\t\tduration : " << duration.numerator() << "/"
                                                 << duration.denominator());
                        Tuplet *tuplet = chord->tuplet();
                        if (tuplet)
                            COUT("\t\t\ttuplet ratio " << tuplet->ratio().numerator() << "/"
                                                       << tuplet->ratio().denominator());

                        for (Note *note : chord->notes())
                        {
                            COUT("\t\t\t\t" << noteToLyPitch(note));
                        }
                    }
                }
            }
        }
    }

    return true;
}

std::string LilyExporter::noteToLyPitch(const Note *note)
{
    int pitch = note->pitch() % 12;
    LyAccidentalName accName;

    switch (note->accidentalType())
    {
        case AccidentalType::NONE:
        case AccidentalType::NATURAL:
            accName = LYNATURAL;
            break;
        case AccidentalType::SHARP:
            pitch -= 1;
            if (pitch < 0)
                pitch += 12;
            accName = LYSHARP;
            break;
        case AccidentalType::SHARP2:
            pitch -= 2;
            if (pitch < 0)
                pitch += 12;
            accName = LYSSHARP;
            break;
        case AccidentalType::FLAT:
            pitch += 1;
            pitch %= 12;
            accName = LYFLAT;
            break;
        case AccidentalType::FLAT2:
            pitch += 2;
            pitch %= 12;
            accName = LYFFLAT;
            break;
        default:
            COUT("AccidentalType not handled yet");
            throw(-1);
            break;
    }

    std::string lyPitch = _pitchToNote[_lang][pitch] + _accidentalName[_lang][accName];

    /// DEBUG ONLY
    if (_pitchToNote[_lang][pitch].empty())
    {
        std::string error("empty note ! initial pitch = ");
        error += std::to_string(note->pitch());
        error += ", computed pitch = ";
        error += std::to_string(pitch);
        throw(error);
    }
    /// END DEBUG

    return lyPitch;
}
} // namespace Ms
