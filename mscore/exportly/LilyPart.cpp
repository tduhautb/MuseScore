#include "LilyPart.hpp"
#include "LilyBarLine.hpp"
#include "LilyClef.hpp"
#include "LilyElement.hpp"
#include "LilyFullMeasureRest.hpp"
#include "LilyMeasure.hpp"
#include "LilyRest.hpp"
#include "LilyTimeSig.hpp"

#include <stack>

using namespace Ms;

LilyPart::LilyPart(const std::string& partName, std::vector<unsigned int> tracks)
    : LilyElement(LILY_PART)
{
    _partName = partName;
    _tracks = tracks;
    for (unsigned int i : tracks)
    {
        _first[i] = nullptr;
        _currentMeasure[i] = nullptr;
        _nbMeasures[i] = 0;
    }
}

LilyMeasure* LilyPart::newMeasure(unsigned int track)
{
    LilyMeasure* mes = new LilyMeasure(++_nbMeasures[track]);

    if (!_first[track])
    {
        _first[track] = mes;
    }
    else
    {
        _currentMeasure[track]->setNext(mes);
        mes->setPrev(_currentMeasure[track]);
    }

    _currentMeasure[track] = mes;

    return mes;
}

// chaque élément gère lui-même ses espaces, tabulations et fins de ligne
std::ofstream& LilyPart::operator>>(std::ofstream& file) const
{
    for (unsigned int i : _tracks)
    {
        std::string trackName = _partName;

        // TODO convert i into letters to avoid too long names
        for (unsigned int j = 0; j < i; j++)
            trackName += 'i';

        file << trackName << " = \\absolute {" << std::endl;
        file << "\t\\compressFullBarRests" << std::endl;
        for (LilyElement* element = _first.at(i); element; element = element->next())
        {
            *element >> file;
        }

        file << "}" << std::endl;

        if (i != *(_tracks.end()))
            file << std::endl;
    }

    return file;
}

void LilyPart::reorganize()
{
    /*----------------------------------------------------------
     *  extract elements from the measures directly in the part
     *  LilyKey
     *  LilyTimeSig
     *  LilyBarLine
     *----------------------------------------------------------*/
    for (unsigned int track : _tracks)
    {
        std::map<unsigned int, LilyElement*> currents = _first;
        while (currents[track])
        {
            LilyElement* current = currents[track];
            LilyMeasure* mes = dynamic_cast<LilyMeasure*>(current);
            if (!mes)
            {
                for (unsigned int i : _tracks)
                    currents[i] = currents[i]->next();
                continue;
            }

            if (mes->isEmpty())
            {
                // fill empty measures with a rest
                Fraction complement;
                if (mes->isAnacrusis())
                {
                    // special care if this is an anacrusis, get the anacrusis fraction from an
                    // other not empty measure
                    Fraction anacrusisFraction;
                    for (unsigned int track : _tracks)
                    {
                        LilyMeasure* parallelMes = dynamic_cast<LilyMeasure*>(currents[track]);
                        if (!parallelMes->isEmpty())
                        {
                            anacrusisFraction = parallelMes->getAnacrusisFraction();
                            complement = mes->getFraction() - anacrusisFraction;
                            break;
                        }
                    }
                }
                else
                {
                    complement = mes->getFraction();
                }

                mes->addElement(new LilyRest(complement));

                if (mes->isAnacrusis())
                    mes->checkAnacrousis();
            }

            std::stack<LilyElement*> extracted;

            extracted.push(mes->extractElement<LilyKey>());
            extracted.push(mes->extractElement<LilyTimeSig>());
            extracted.push(mes->extractElement<LilyBarLine>());

            while (!extracted.empty())
            {
                LilyElement* extractedElement = extracted.top();
                extracted.pop();

                if (!extractedElement)
                    continue;

                switch (extractedElement->getType())
                {
                    case LILY_KEY:
                    case LILY_TIMESIG:
                        for (unsigned int i : _tracks)
                        {
                            LilyElement* copied;
                            if (extractedElement->getType() == LILY_TIMESIG)
                                copied = new LilyTimeSig(
                                    dynamic_cast<const LilyTimeSig*>(extractedElement));
                            else
                                copied =
                                    new LilyKey(dynamic_cast<const LilyKey*>(extractedElement));

                            // extract before the measure
                            copied->setNext(currents[i]);
                            copied->setPrev(currents[i]->prev());

                            if (currents[i]->prev())
                                currents[i]->prev()->setNext(copied);

                            if (currents[i] == _first[i])
                                _first[i] = copied;

                            currents[i]->setPrev(copied);
                        }
                        break;
                    case LILY_BARLINE:
                        // extract after the measure
                        for (unsigned int i : _tracks)
                        {
                            LilyBarLine* copied =
                                new LilyBarLine(dynamic_cast<const LilyBarLine*>(extractedElement));
                            copied->setPrev(currents[i]);
                            copied->setNext(currents[i]->next());

                            if (currents[i]->next())
                                currents[i]->next()->setPrev(copied);

                            currents[i]->setNext(copied);
                        }
                        break;
                    default:
                        break;
                }

                delete extractedElement;
            }

            for (unsigned int i : _tracks)
            {
                currents[i] = currents[i]->next();
            }
        }
    }

    // elements to delete at the end
    std::stack<LilyElement*> toDelete;

    for (unsigned int track : _tracks)
    {
        /*----------------------------------------------------------
         * Simplify the extracted elements to suppress duplicates
         *----------------------------------------------------------*/
        const LilyClef* currentClef = nullptr; // clé
        const LilyKey* currentKey = nullptr;   // armure
        const LilyTimeSig* currentTimeSig = nullptr;

        for (LilyElement* current = _first[track]; current; current = current->next())
        {
            /*----------------------------------------------------------
             *  Simplification process is different depending on the objects
             *----------------------------------------------------------*/
            switch (current->getType())
            {
                case LILY_MEASURE:
                {
                    LilyMeasure* mes = dynamic_cast<LilyMeasure*>(current);
                    // remove the clef at the beginning if unnecessary
                    mes->simplify(&currentClef);

                    // is the measure is a full bar rest, we create a new object or use the
                    // precedent full measure rest to compress
                    if (mes->isFullBarRest())
                    {
                        if (mes->prev() && mes->prev()->getType() == LILY_FULLMEASUREREST &&
                            mes->prev()->getFraction() == mes->getFraction())
                        {
                            LilyFullMeasureRest* prevRest =
                                dynamic_cast<LilyFullMeasureRest*>(mes->prev());

                            // both measures are the same, compress them
                            prevRest->addFullMeasure(1);

                            // disconnect the measures
                            prevRest->setNext(mes->next());
                            if (mes->next())
                                mes->next()->setPrev(prevRest);
                        }
                        else
                        {
                            LilyFullMeasureRest* fullRest =
                                new LilyFullMeasureRest(mes->getFraction(), mes->getMeasureNum());
                            fullRest->setPrev(mes->prev());
                            fullRest->setNext(mes->next());
                            if (mes->prev())
                                mes->prev()->setNext(fullRest);
                            if (mes->next())
                                mes->next()->setPrev(fullRest);
                        }
                        toDelete.push(mes);
                    }
                    break;
                }
                case LILY_KEY:
                    if (simplify(dynamic_cast<const LilyKey*>(current), &currentKey, track))
                        toDelete.push(current);
                    break;
                case LILY_TIMESIG:
                    if (simplify(dynamic_cast<const LilyTimeSig*>(current), &currentTimeSig, track))
                        toDelete.push(current);
                    break;
                case LILY_BARLINE:
                    if (current->next() && current->next()->getType() == LILY_BARLINE)
                    {
                        toDelete.push(current->next());
                        // déconnecter la barline dupliquée
                        current->setNext(current->next()->next());
                        if (current->next())
                            current->next()->setPrev(current);
                    }
                default:
                    break;
            }
        }
    }

    while (!toDelete.empty())
    {
        delete toDelete.top();
        toDelete.pop();
    }
}

void LilyPart::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << "Part : " << std::endl;

    for (unsigned int track : _tracks)
    {
        std::cout << "\ttrack " << std::to_string(track) << std::endl;
        for (const LilyElement* current = _first.at(track); current; current = current->next())
            current->log(indentation + 1);
    }
}
