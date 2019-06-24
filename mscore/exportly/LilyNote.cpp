#include "LilyNote.hpp"
#include "mscore/exportly.hpp"

using namespace Ms;

const std::string Ms::LilyNote::_pitchToNote[2][7] = {{"a", "b", "c", "d", "e", "f", "g"},
                                                      {"la", "si", "do", "re", "mi", "fa", "sol"}};
const std::string Ms::LilyNote::_accidentalName[2][5] = {{"", "is", "es", "isis", "eses"},
                                                         {"", "d", "b", "dd", "bb"}};

LilyNote::LilyNote(const Chord* chord) : LilyElement(LILY_NOTE), _chord(chord)
{
}

std::ofstream& LilyNote::operator>>(std::ofstream& file) const
{
    if (_chord->notes().size() > 1)
        file << "<";

    for (Note* note : _chord->notes())
    {
        file << noteToLy(note);
        if (note != _chord->notes().back())
            file << " ";
    }

    if (_chord->notes().size() > 1)
        file << ">";

    file << LilyExporter::lilyDuration(_chord->ticks());

    return file;
}

std::string LilyNote::noteToLy(const Note* note) const
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
                // todo throw exception
                std::cerr << "UNKNOWN ALTERATION";
                break;
        }
    }

    LilyExporter::OutputLanguage lang = LilyExporter::getInstance()->getLang();
    std::string lyPitch = _pitchToNote[lang][pitch];
    lyPitch += _accidentalName[lang][accName];
    int height = (note->epitch() / 12) - 4;

    for (int i = height; i < 0; i++)
        lyPitch += ",";

    for (int i = 0; i < height; i++)
        lyPitch += "'";

    return lyPitch;
}

Fraction LilyNote::getFraction() const
{
    return _chord->ticks();
}

void LilyNote::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _chord->accessibleInfo().toStdString() << std::endl;
}
