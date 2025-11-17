//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_UTILIZATOR_H
#define OOP_UTILIZATOR_H

#include <string>
#include <vector>
#include <iostream>
#include "RezultatIntrebare.h"


class Utilizator {

    std::string nume;
    int scor;
    bool ajutorDisponibil = true;
    int ajutoareFolosite = 0;
    std::vector<RezultatIntrebare> istoric;

public:
    explicit Utilizator(const std::string& nume = "Anonim", int scor = 0);

    void adaugaScor(int puncte);
    int getScor() const;
    const std::string& getNume() const;

    bool poateFolosiAjutor() const;
    void consumaAjutor();

    void adaugaRezultat(const RezultatIntrebare& rez);

    int getAjutoareFolosite() const;
    const std::vector<RezultatIntrebare>& getIstoric() const;

    void afiseazaIstoric() const;

    friend std::ostream& operator<<(std::ostream& out, const Utilizator& u);
};

#endif //OOP_UTILIZATOR_H