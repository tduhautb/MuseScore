#include "LilyUtils.hpp"

std::string Ms::romanUnit(unsigned int i, char unit, char five, char nextUnit)
{
    std::string val;

    switch (i)
    {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
            for (unsigned int j = 0; j < i; j++)
                val += unit;
            break;
        case 4:
            val += unit;
            val += five;
            break;
        case 5:
            val += five;
            break;
        case 6:
        case 7:
        case 8:
            val += five;
            for (unsigned int j = 5; j < i; j++)
                val += unit;
            break;
        case 9:
            val += unit;
            val += nextUnit;
            break;
    }

    return val;
}

std::string Ms::intToRoman(unsigned int i)
{
    std::string roman;

    unsigned int unit = i % 10;
    unsigned int ten = (i / 10);
    unsigned int hunds = (i / 100);
    unsigned int thousd = (i / 1000);

    for (unsigned int i = 0; i < thousd; i++)
        roman += "M";

    roman += Ms::romanUnit(hunds, 'C', 'D', 'M');
    roman += Ms::romanUnit(ten, 'X', 'L', 'C');
    roman += Ms::romanUnit(unit, 'I', 'V', 'X');

    return roman;
}
