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
    // streak curent de raspunsuri corecte consecutive
    int streakCurent = 0;
    // cel mai mare streak atins in aceasta sesiune
    int streakMax = 0;

public:
    explicit Utilizator(const std::string& nume = "Anonim", int scor = 0);

    void adaugaScor(int puncte);
    int getScor() const noexcept;
    const std::string& getNume() const noexcept;

    bool poateFolosiAjutor() const noexcept;
    void consumaAjutor();

    void adaugaRezultat(const RezultatIntrebare& rez);

    int getAjutoareFolosite() const noexcept;
    const std::vector<RezultatIntrebare>& getIstoric() const noexcept;

    // actualizeaza streak-ul in functie de raspuns
    void inregistreazaRaspuns(bool corect, bool sarita) noexcept;
    // calculeaza bonusul pentru streak pe raspunsul curent; multiplicatorul tine cont de tipul intrebarii
    int calculeazaBonusStreak(int multiplicator) const noexcept;
    int getStreakCurent() const noexcept { return streakCurent; }
    int getStreakMax() const noexcept { return streakMax; }

    void afiseazaIstoric() const;

    friend std::ostream& operator<<(std::ostream& out, const Utilizator& u);
};

#endif //OOP_UTILIZATOR_H