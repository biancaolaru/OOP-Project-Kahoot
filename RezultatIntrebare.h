//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_REZULTATINTREBARE_H
#define OOP_REZULTATINTREBARE_H

#pragma once
#include <string>
#include <vector>

struct RezultatIntrebare {
    std::string intrebare;
    std::vector<int> raspunsuriAlese;
    bool corecta = false;
    bool sarita = false;
};


#endif //OOP_REZULTATINTREBARE_H