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

#include <QMessageBox>

#include "exportly/LilyArticulation.hpp"
#include "exportly/LilyBarLine.hpp"
#include "exportly/LilyClef.hpp"
#include "exportly/LilyDynamic.hpp"
#include "exportly/LilyElement.hpp"
#include "exportly/LilyExportDialog.hpp"
#include "exportly/LilyKey.hpp"
#include "exportly/LilyMeasure.hpp"
#include "exportly/LilyNote.hpp"
#include "exportly/LilyPart.hpp"
#include "exportly/LilyRest.hpp"
#include "exportly/LilySpanner.hpp"
#include "exportly/LilyTimeSig.hpp"
#include "exportly/LilyTuplet.hpp"
#include "exportly/LilyUtils.hpp"

#include "libmscore/accidental.h"
#include "libmscore/articulation.h"
#include "libmscore/barline.h"
#include "libmscore/chord.h"
#include "libmscore/clef.h"
#include "libmscore/dynamic.h"
#include "libmscore/fraction.h"
#include "libmscore/hairpin.h"
#include "libmscore/measure.h"
#include "libmscore/measurebase.h"
#include "libmscore/part.h"
#include "libmscore/rest.h"
#include "libmscore/score.h"
#include "libmscore/scoreElement.h"
#include "libmscore/spanner.h"
#include "libmscore/spannermap.h"
#include "libmscore/staff.h"
#include "libmscore/sym.h"
#include "libmscore/timesig.h"
#include "libmscore/tuplet.h"
#include "libmscore/types.h"
#include "musescore.h"

#define COUT(txt) std::cout << txt << std::endl

Ms::LilyExporter* Ms::LilyExporter::_instance = nullptr;

namespace Ms
{

bool saveLy(Score* score, const QString& name, QMainWindow* mscore)
{
    try
    {
        return LilyExporter::createInstance(score, name, mscore)->exportFile();
    }
    catch (const std::exception& e)
    {
        COUT(e.what());
    }
    catch (...)
    {
        COUT("exception caught");
    }

    LilyExporter::freeInstance();

    return false;
}

LilyExporter* LilyExporter::createInstance(Score* score, const QString& filename,
                                           QMainWindow* mscore)
{
    if (_instance)
        return _instance;

    _instance = new LilyExporter(score, filename, mscore);

    return getInstance();
}

LilyExporter* LilyExporter::getInstance()
{
    return _instance;
}

void LilyExporter::freeInstance()
{
    if (_instance)
        delete _instance;

    _instance = nullptr;
}

LilyExporter::LilyExporter(Score* score, const QString& filename, QMainWindow* mscore)
    : _score(score), _filename(filename), _mscore(mscore)
{
}

bool LilyExporter::exportFile()
{
    requestOptions();

    // truncate existing file
    _outputFile.open(_filename.toStdString(), ios::trunc);

    printLilyHeaders();

    // iterate over each part
    QList<Part*> parts = _score->parts();
    for (int iPart = 0; iPart < parts.size(); iPart++)
    {
        processPart(parts[iPart]);
        newline(); // line return after the last '}' of the music
        newline(); // blank line for better reading
    }

    print("\\score {");
    newline();
    print("<<");

    for (int iPart = 0; iPart < parts.size(); iPart++)
    {
        newline();
        printPartStaff(parts[iPart]);
        newline();
    }

    print(">>");
    newline();
    print("}");

    closeFile();

    // display a message to the user

    QMessageBox msgBox;
    msgBox.setText("The export ended successfully !");
    msgBox.setWindowTitle("Export successful");
    msgBox.exec();

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
    if (_options._lang == OutputLanguage::ITALIANO)
    {
        print("\\language \"italiano\"");
        newline();
    }

    print("\\paper {");
    newline();
    print("\tline-width = 180\\mm");
    newline();
    print("\tleft-margin = 20\\mm");
    newline();
    print("\ttop-margin = 10\\mm");
    newline();
    print("\tbottom-margin = 20\\mm");
    newline();
    print("\tragged-last-bottom = ##f");
    newline();
    print("}");
    newline();
    newline();

    print("\\header {");
    newline();
    print("\ttitle = \"" + _score->metaTag("workTitle").toStdString() + "\"");
    newline();
    print("}");
    newline();
    newline();
}

// static function
std::string LilyExporter::lilyDuration(Fraction frac)
{
    frac.reduce();
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

std::string LilyExporter::generatePartName(const Part* part)
{
    std::string instrumentName = part->instrumentName().toStdString();

    // remove eventual spaces
    std::vector<int> toRemove;
    for (unsigned int i = 0; i < instrumentName.length(); i++)
    {
        switch (instrumentName[i])
        {
            case ' ':
            case '(':
            case ')':
                toRemove.push_back(i);
                break;
            default:
                break;
        }
    }

    for (int i = toRemove.size() - 1; i >= 0; i--)
        instrumentName.erase(toRemove[i], 1);

    // append the track to the instrument
    std::string tmpName = instrumentName;
    char added = 'a';

    // check potential conflict with existing instrument
    while (_partNames.find(tmpName) != _partNames.end())
    {
        tmpName = instrumentName + added;
        added++;
    }

    return tmpName;
}

/*----------------------------------------------------------
 * Global processing functions
 *----------------------------------------------------------*/

void LilyExporter::processPart(const Part* part)
{
    MeasureBase* measure;
    std::vector<unsigned int> usedTracks;
    getUsedTracks(part, usedTracks);

    std::string partName = generatePartName(part);
    _partNames.insert(partName);
    _partToName[part] = partName;

    LilyPart lilyPart(partName, usedTracks);

    // iterate over the tracks of the part
    for (int track : usedTracks)
    {
        // iterate over the measures of the score
        for (measure = _score->measures()->first(); measure; measure = measure->next())
        {
            if (measure->type() != ElementType::MEASURE)
                continue;

            LilyMeasure* lilyMeasure = lilyPart.newMeasure(track);

            Measure* mes = dynamic_cast<Measure*>(measure);

            lilyMeasure->setFraction(mes->timesig());

            bool chordFound = false;

            LilyTuplet* lastTuplet = nullptr;
            Tuplet* lastMsTuplet = nullptr;

            // collect the chords of the measure for the current track
            for (Segment* seg = mes->first(); seg; seg = seg->next())
            {
                Element* element = seg->element(track);
                if (!element)
                    continue;

                if (element->type() == ElementType::CHORD || element->type() == ElementType::REST)
                    chordFound = true;

                // try to get the duration element
                DurationElement* eventualTuplet = dynamic_cast<DurationElement*>(element);
                if (eventualTuplet && eventualTuplet->tuplet())
                {
                    // the duration element exists and is a tuplet
                    if (lastMsTuplet != eventualTuplet->tuplet())
                    {
                        // the tuplet eventualTuplet does not refer to the last tuplet used, in this
                        // case we have multiple consecutive tuplets

                        // store the last MuseScore tuplet
                        lastMsTuplet = eventualTuplet->tuplet();

                        // define a new LilyPond LilyTuplet element
                        lastTuplet = dynamic_cast<LilyTuplet*>(processElement(lastMsTuplet));

                        // add the tuplet to the measure
                        lilyMeasure->addElement(lastTuplet);
                    }

                    // the tuplet already exists and is already stored in the measure, we have to
                    // fill the tuplet
                    lastTuplet->addElement(processElement(element));
                }
                else
                {
                    // there is no duration element or the element is not a tuplet
                    if (lastTuplet)
                    {
                        // the lastTuplet is over, we reset it to nullptr
                        lastTuplet = nullptr;
                        lastMsTuplet = nullptr;
                    }

                    // add the current element to the measure
                    lilyMeasure->addElement(processElement(element));
                }

                if (element->type() == ElementType::CHORD)
                {
                    Chord* chord = dynamic_cast<Chord*>(element);
                    const QVector<Articulation*>& articulations = chord->articulations();

                    for (Articulation* art : articulations)
                    {
                        // the articulation must be added in the tuplet if it exists, on the measure
                        // otherwise
                        if (lastTuplet)
                            lastTuplet->addElement(processElement(art));
                        else
                            lilyMeasure->addElement(processElement(art));
                    }
                }

                std::vector<Element*> annotations =
                    seg->findAnnotations(ElementType::DYNAMIC, track, track);

                if (annotations.size() > 1)
                {
                    for (unsigned int i = 1; i < annotations.size();)
                    {
                        Dynamic* dyn1 = dynamic_cast<Dynamic*>(annotations[i - 1]);
                        Dynamic* dyn2 = dynamic_cast<Dynamic*>(annotations[i]);
                        if (dyn1->dynamicType() == dyn2->dynamicType())
                        {
                            std::vector<Element*>::iterator it = annotations.begin() + i;
                            annotations.erase(it);
                        }
                        else
                        {
                            i++;
                        }
                    }
                }

                for (const Element* annotation :
                     seg->findAnnotations(ElementType::DYNAMIC, track, track))
                {
                    // the annotation must be added in the tuplet if it exists, on the measure
                    // otherwise
                    if (lastTuplet)
                        lastTuplet->addElement(processElement(annotation));
                    else
                        lilyMeasure->addElement(processElement(annotation));
                }

                if (lastTuplet)
                    lastTuplet->addElement(checkSpanner(element));
                else
                    lilyMeasure->addElement(checkSpanner(element));

                // don't process elements after the last bar
                if (chordFound && element->type() == ElementType::BAR_LINE)
                    break;
            }

            lilyMeasure->checkAnacrousis();
        }
    }

    lilyPart.reorganize();
    lilyPart >> _outputFile;
}

LilyElement* LilyExporter::processElement(const Element* element)
{
    // COUT("element " << element->accessibleInfo().toStdString());
    switch (element->type())
    {
        case ElementType::CHORD:
            return new LilyNote(dynamic_cast<const Chord*>(element));
        case ElementType::REST:
            return new LilyRest(dynamic_cast<const Rest*>(element));
        case ElementType::CLEF:
            return new LilyClef(dynamic_cast<const Clef*>(element));
        case ElementType::KEYSIG:
            return new LilyKey(dynamic_cast<const KeySig*>(element));
        case ElementType::TIMESIG:
            return new LilyTimeSig(dynamic_cast<const TimeSig*>(element));
        case ElementType::BAR_LINE: {
            const BarLine* barline = dynamic_cast<const BarLine*>(element);
            if (barline->barLineType() == BarLineType::NORMAL)
                return nullptr;
            return new LilyBarLine(barline);
        }
        case ElementType::DYNAMIC:
            return new LilyDynamic(dynamic_cast<const Dynamic*>(element));
        case ElementType::ARTICULATION:
            return new LilyArticulation(dynamic_cast<const Articulation*>(element));
        case ElementType::TUPLET:
            return new LilyTuplet(dynamic_cast<const Tuplet*>(element));
        default:
            return nullptr;
    }
}

void LilyExporter::getUsedTracks(const Part* part, std::vector<unsigned int>& tracks)
{
    _partToStaves[part] = std::vector<const Staff*>();
    for (const Staff* staff : *part->staves())
    {
        _partToStaves[part].push_back(staff);
        _staffToTracks[staff] = std::vector<unsigned int>();
    }

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

                if (element->type() == ElementType::CHORD || element->type() == ElementType::REST)
                {
                    _staffToTracks[element->staff()].push_back(track);
                    tracks.push_back(track);
                    trackUsed = true;
                }
            }
        }
    }
}

void LilyExporter::printPartStaff(const Part* part)
{
    const std::string& partName = _partToName[part];
    std::string staffType;

    switch (_partToStaves[part].size())
    {
        case 1:
            // only 1 staff
            staffType = "Staff";
            break;
        default:
            // 2 or more staves
            staffType = "PianoStaff";
            break;
    }

    print("\t\\context " + staffType + " = \"" + partName + "\"");
    newline();
    print("\t<<");
    newline();
    print("\t\t\\set " + staffType + ".instrumentName = \"" + part->longName().toStdString() +
          "\"");
    newline();
    print("\t\t\\set " + staffType + ".shortInstrumentName = \"" + part->shortName().toStdString() +
          "\"");
    newline();
    for (const Staff* staff : _partToStaves[part])
        printTracks(partName, _staffToTracks[staff]);
    print("\t>>");
}

void LilyExporter::printTracks(const std::string partName, const std::vector<unsigned int>& tracks)
{
    switch (tracks.size())
    {
        case 1: {
            std::string partTrack = partName + intToRoman(tracks[0]);

            print("\t\t\\context Voice = \"" + partTrack + "\" { \\" + partTrack + " }");
            newline();
            break;
        }
        case 2: {
            std::string partTrack1 = partName + intToRoman(tracks[0]);
            std::string partTrack2 = partName + intToRoman(tracks[1]);

            print("\t\t\\context Voice = \"" + partName + "\" { \\partcombine \\" + partTrack1 +
                  " \\" + partTrack2 + " }");
            newline();
            break;
        }
        default:
            std::cerr << "3 and more tracks not handled yet !" << std::endl;
            std::cerr << "result : " << tracks.size() << std::endl;
            break;
    }
}

std::vector<LilyElement*> LilyExporter::checkSpanner(const Element* element)
{
    std::vector<LilyElement*> retSpanners;

    if (element->type() != ElementType::CHORD && element->type() != ElementType::REST)
    {
        // we check if this is not the final double-bar which may hold the end of the last slur
        if (!(element->type() == ElementType::BAR_LINE &&
              dynamic_cast<const BarLine*>(element)->barLineType() == BarLineType::END))
            return retSpanners;
    }

    SpannerMap& smap = _score->spannerMap();
    auto spanners = smap.findOverlapping(element->tick().ticks(), element->tick().ticks());

    for (auto interval : spanners)
    {
        Spanner* s = interval.value;

        switch (s->type())
        {
            case ElementType::SLUR:
                if (s->tick() == element->tick() && s->track() == element->track())
                    retSpanners.push_back(new LilySpanner(LilySpanner::SpannerType::SLUR_BEGIN));

                if (s->tick2() == element->tick() && s->track2() == element->track())
                    retSpanners.push_back(new LilySpanner(LilySpanner::SpannerType::SLUR_END));
                break;
            case ElementType::HAIRPIN:
                if (s->tick() == element->tick() && s->track() == element->track())
                {
                    Hairpin* hairpin = dynamic_cast<Hairpin*>(s);
                    switch (hairpin->hairpinType())
                    {
                        case HairpinType::CRESC_HAIRPIN:
                            retSpanners.push_back(
                                new LilySpanner(LilySpanner::SpannerType::CRESC_HAIRPIN));
                            break;
                        case HairpinType::DECRESC_HAIRPIN:
                            retSpanners.push_back(
                                new LilySpanner(LilySpanner::SpannerType::DECRESC_HAIRPIN));
                            break;
                        case HairpinType::CRESC_LINE:
                            retSpanners.push_back(new LilySpanner(LilySpanner::SpannerType::CRESC));
                            break;
                        case HairpinType::DECRESC_LINE:
                            retSpanners.push_back(
                                new LilySpanner(LilySpanner::SpannerType::DECRESC));
                            break;
                        default:
                            break;
                    }
                }
                if (s->tick2() == element->tick() && s->track2() == element->track())
                    retSpanners.push_back(new LilySpanner(LilySpanner::SpannerType::END));
                break;
            default:
                // std::cout << "spanner : " << s->accessibleInfo().toStdString() << std::endl;
                // std::cout << "type : " << s->name() << std::endl;
                break;
        }
    }

    return retSpanners;
}

LilyExporter::OutputLanguage LilyExporter::getLang() const
{
    return _options._lang;
}

void LilyExporter::requestOptions()
{
    // create a dialog window to get user options
    LilyExportDialog* dialog = new LilyExportDialog(_mscore);
    dialog->exec();

    _options = dialog->getOptions();

    delete dialog;
}

const LilyExporter::LilyExportOptions& LilyExportDialog::getOptions() const
{
    return _options;
}
} // namespace Ms
