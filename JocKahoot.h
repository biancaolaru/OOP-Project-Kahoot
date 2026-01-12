//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_JOCKAHOOT_H
#define OOP_JOCKAHOOT_H

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "Utilizator.h"
#include "Quiz.h"
#include "Intrebare.h"
#pragma once

class JocKahoot {

    std::vector<Utilizator> utilizatori;
    Quiz chestionar;
    // meta-date ale sesiunii
    int dificultateSelectata = 0;
    size_t numarIntrebariSelectate = 0;

public:
    explicit JocKahoot(Quiz&& c);

    // seteaza meta-datele sesiunii (dificultate si numarul de intrebari selectate)
    void setMetaJoc(int dificultate, size_t numarIntrebari) noexcept;

    void adaugaUtilizator(const Utilizator& u);
    void startJoc(std::istream& in = std::cin);
    void afiseazaRaportGlobal() const;

    friend std::ostream& operator<<(std::ostream& out, const JocKahoot& j);
};


#endif //OOP_JOCKAHOOT_H