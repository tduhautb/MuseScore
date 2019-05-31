#include "LilyPart.hpp"
#include "LilyBarLine.hpp"
#include "LilyClef.hpp"
#include "LilyElement.hpp"
#include "LilyMeasure.hpp"
#include "LilyTimeSig.hpp"

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
    const LilyClef* currentClef = nullptr; // clé
    const LilyKey* currentKey = nullptr;   // armure
    const LilyTimeSig* currentTimeSig = nullptr;

    // remove duplicates clefs, keys and time signatures
    for (LilyElement* current = _first; current; current = current->next())
    {
        LilyMeasure* mes = dynamic_cast<LilyMeasure*>(current);
        if (!mes)
            continue;

        mes->simplify(&currentClef);
        mes->simplify(&currentKey);
        mes->simplify(&currentTimeSig);
        mes->checkAnacrousis(currentTimeSig);
        mes->compressRests(currentTimeSig->getFraction());
    }

    // extract elements from the measures directly in the part :
    // LilyKey
    // LilyTimeSig
    // LilyBar
    for (LilyElement* current = _first; current; current = current->next())
    {
        LilyMeasure* mes = dynamic_cast<LilyMeasure*>(current);
        if (!mes)
            continue;

        std::vector<LilyElement*> extracted;

        extracted.push_back(mes->extractElement<LilyKey>());
        extracted.push_back(mes->extractElement<LilyTimeSig>());
        extracted.push_back(mes->extractElement<LilyBarLine>());

        for (LilyElement* extractedElement : extracted)
        {
            if (!extractedElement)
                continue;

            switch (extractedElement->getType())
            {
                case LILY_BARLINE:
                case LILY_KEY:
                    // extract after the current measure
                    extractedElement->setPrev(current);
                    extractedElement->setNext(current->next());

                    if (current->next())
                        current->next()->setPrev(extractedElement);

                    current->setNext(extractedElement);
                    break;
                case LILY_TIMESIG:
                    // extract before the current measure
                    extractedElement->setNext(current);
                    extractedElement->setPrev(current->prev());

                    if (current->prev())
                        current->prev()->setNext(extractedElement);

                    current->setPrev(extractedElement);
                    break;
                default:
                    break;
            }
        }
    }
}
