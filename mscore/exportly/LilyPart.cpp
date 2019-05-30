#include "LilyPart.hpp"
#include "LilyClef.hpp"
#include "LilyElement.hpp"
#include "LilyMeasure.hpp"
#include "LilyTimeSig.hpp"

using namespace Ms;

LilyPart::LilyPart(const std::string& partName) : LilyElement()
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
        _currentMeasure = mes;
    }

    _currentMeasure->setNext(mes);
    mes->setPrev(_currentMeasure);
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
    }

    // when changing an armor, make sure the object is at the beginning of the next measure.
    // we don't process the first measure to keep the initial armor in place
    for (LilyElement* current = _first->next(); current; current = current->next())
    {
        LilyMeasure* mes = dynamic_cast<LilyMeasure*>(current);
        if (!mes)
            continue;

        mes->moveKeyToNextMeasure();
    }
}
