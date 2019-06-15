#include "LilyKey.hpp"
#include "LilyNote.hpp"
#include "mscore/exportly.hpp"

#include "libmscore/keysig.h"

using namespace Ms;

LilyKey::LilyKey(const KeySig* keySig) : LilyElement(LILY_KEY), _keySig(keySig)
{
}

std::ofstream& LilyKey::operator>>(std::ofstream& file) const
{
    file << "\t\\key ";
    int pitch = 0;
    LilyNote::LyAccidentalName accidental = LilyNote::LYNATURAL;
    switch (_keySig->key())
    {
        case Key::A:
        case Key::A_B:
            pitch = 0;
            break;
        case Key::B:
        case Key::B_B:
            pitch = 1;
            break;
        case Key::C:
        case Key::C_B:
        case Key::C_S:
            pitch = 2;
            break;
        case Key::D:
        case Key::D_B:
            pitch = 3;
            break;
        case Key::E:
        case Key::E_B:
            pitch = 4;
            break;
        case Key::F:
        case Key::F_S:
            pitch = 5;
            break;
        case Key::G:
        case Key::G_B:
            pitch = 6;
            break;
        default:
            pitch = 0;
            break;
    }

    LilyExporter::OutputLanguage lang = LilyExporter::getInstance()->getLang();
    file << LilyNote::_pitchToNote[lang][pitch];

    switch (_keySig->key())
    {
        case Key::A_B:
        case Key::B_B:
        case Key::C_B:
        case Key::D_B:
        case Key::E_B:
            accidental = LilyNote::LYFLAT;
            break;
        case Key::C_S:
        case Key::F_S:
            accidental = LilyNote::LYSHARP;
            break;
        default:
            break;
    }

    file << LilyNote::_accidentalName[0][accidental] << " ";

    switch (_keySig->keySigEvent().mode())
    {
        case KeyMode::NONE:
        case KeyMode::MAJOR:
        case KeyMode::UNKNOWN:
            file << "\\major";
            break;
        case KeyMode::MINOR:
            file << "\\minor";
    }

    file << std::endl;
    return file;
}

bool LilyKey::operator==(const LilyKey& other) const
{
    return _keySig->key() == other._keySig->key();
}

bool LilyKey::operator!=(const LilyKey& other) const
{
    return !(*this == other);
}

void LilyKey::log(unsigned int indentation) const
{
    for (unsigned int i = 0; i < indentation; i++)
        std::cout << "\t";

    std::cout << name() << " : " << _keySig->accessibleInfo().toStdString() << std::endl;
}
