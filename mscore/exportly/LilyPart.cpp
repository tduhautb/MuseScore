#include "LilyPart.hpp"
#include "LilyBarLine.hpp"
#include "LilyClef.hpp"
#include "LilyElement.hpp"
#include "LilyFullMeasureRest.hpp"
#include "LilyMeasure.hpp"
#include "LilyTimeSig.hpp"

#include <stack>

using namespace Ms;

LilyPart::LilyPart(const std::string& partName) : LilyElement(LILY_PART)
{
    _first = nullptr;
    _currentMeasure = nullptr;
    _nbMeasures = 0;
    _partName = partName;
}

LilyMeasure* LilyPart::newMeasure()
{
    LilyMeasure* mes = new LilyMeasure(++_nbMeasures);

    if (!_first)
    {
        _first = mes;
    }
    else
    {
        _currentMeasure->setNext(mes);
        mes->setPrev(_currentMeasure);
    }

    _currentMeasure = mes;

    return mes;
}

// chaque élément gère lui-même ses espaces, tabulations et fins de ligne
std::ofstream& LilyPart::operator>>(std::ofstream& file) const
{
    file << _partName << " = \\absolute {" << std::endl;
    file << "\t\\compressFullBarRests" << std::endl;
    for (LilyElement* element = _first; element; element = element->next())
    {
        *element >> file;
    }

    file << "}" << std::endl;

    if (next())
        file << std::endl;

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
    for (LilyElement* current = _first; current; current = current->next())
    {
        LilyMeasure* mes = dynamic_cast<LilyMeasure*>(current);
        if (!mes)
            continue;

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
                    // extract before the measure
                    extractedElement->setNext(current);
                    extractedElement->setPrev(current->prev());

                    if (current->prev())
                        current->prev()->setNext(extractedElement);

                    if (current == _first)
                        _first = extractedElement;

                    current->setPrev(extractedElement);
                    break;
                case LILY_BARLINE:
                    // extract after the measure
                    extractedElement->setPrev(current);
                    extractedElement->setNext(current->next());

                    if (current->next())
                        current->next()->setPrev(extractedElement);

                    current->setNext(extractedElement);
                    break;
                default:
                    break;
            }
        }
    }

    /*----------------------------------------------------------
     * Simplify the extracted elements to suppress duplicates
     *----------------------------------------------------------*/
    const LilyClef* currentClef = nullptr; // clé
    const LilyKey* currentKey = nullptr;   // armure
    const LilyTimeSig* currentTimeSig = nullptr;

    // elements to delete at the end
    std::stack<LilyElement*> toDelete;

    for (LilyElement* current = _first; current; current = current->next())
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

                // store the time signature in the measure
                mes->setFraction(currentTimeSig->getFraction());

                // check an eventual anacrousis
                mes->checkAnacrousis();

                // is the measure is a full bar rest, we create a new object or use the precedent
                // full measure rest to compress
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
                if (simplify(dynamic_cast<const LilyKey*>(current), &currentKey))
                    toDelete.push(current);
                break;
            case LILY_TIMESIG:
                if (simplify(dynamic_cast<const LilyTimeSig*>(current), &currentTimeSig))
                    toDelete.push(current);
                break;
            default:
                break;
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

    for (LilyElement* current = _first; current; current = current->next())
        current->log(indentation + 1);
}
