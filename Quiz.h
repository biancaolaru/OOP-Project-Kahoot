//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_QUIZ_H
#define OOP_QUIZ_H

#include <vector>
#include <memory>
#include <algorithm>
#include "Intrebare.h"

class Quiz {
private:
    std::vector<std::unique_ptr<Intrebare>> intrebari;

public:
    // Constructor care primește vector de unique_ptr prin rvalue-ref (mutare)
    Quiz(std::vector<std::unique_ptr<Intrebare>>&& intrebariVec) {
        for (auto& i : intrebariVec)
            intrebari.push_back(std::move(i));
    }

    // Accesare întrebări (ne-const pentru a permite mutarea elementelor la nevoie)
    std::vector<std::unique_ptr<Intrebare>>& getIntrebari() {
        return intrebari;
    }

    // Amestecă întrebările
    void amestecaIntrebari();

    // Supraincarcare operator<< pentru afisare
    friend std::ostream& operator<<(std::ostream& out, const Quiz& q) {
        for (const auto& i : q.intrebari)
            i->afiseaza();
        return out;
    }
};

#endif //OOP_QUIZ_H
