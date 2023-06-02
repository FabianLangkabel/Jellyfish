#pragma once

#include <vector>

class SDInteraction
{
public:
    SDInteraction(int SD1, int SD2, std::vector<int> Orbitals, bool PositivSign)
    {
        this->SD1 = SD1;
        this->SD2 = SD2;
        this->Orbitals = Orbitals;
        this->PositivSign = PositivSign;
    }
    int SD1;
    int SD2;
    std::vector<int> Orbitals;
    bool PositivSign;
};