//
// Created by Bianca on 11/17/2025.
//

#include "Utilizator.h"
#include <iostream>

Utilizator::Utilizator(const std::string& nume, int scor)
    : nume(nume), scor(scor) {}

void Utilizator::adaugaScor(int puncte) { scor += puncte; }
int Utilizator::getScor() const { return scor; }
const std::string& Utilizator::getNume() const { return nume; }

bool Utilizator::poateFolosiAjutor() const { return ajutorDisponibil; }

void Utilizator::consumaAjutor() {
    if (ajutorDisponibil) {
        ajutorDisponibil = false;
        ++ajutoareFolosite;
    }
}

void Utilizator::adaugaRezultat(const RezultatIntrebare& rez) {
    istoric.push_back(rez);
}

int Utilizator::getAjutoareFolosite() const { return ajutoareFolosite; }

const std::vector<RezultatIntrebare>& Utilizator::getIstoric() const {
    return istoric;
}

void Utilizator::afiseazaIstoric() const {
    std::cout << "\n   >> Raport pentru " << nume << " (" << scor << " puncte"
        << ", Hint folosit: " << (ajutoareFolosite > 0 ? "DA" : "NU") << ")\n";

    if (istoric.empty()) {
        std::cout << "      Nu exista intrebari parcurse in aceasta sesiune.\n";
        return;
    }

    for (size_t i = 0; i < istoric.size(); ++i) {
        const auto& rez = istoric[i];
        std::cout << "      " << i + 1 << ". " << rez.intrebare << "\n";

        if (rez.sarita) {
            std::cout << "         - Intrebarea a fost sarita\n";
            continue;
        }

        if (rez.raspunsuriAlese.empty()) {
            std::cout << "         - Nu a fost introdus niciun raspuns\n";
        }
        else {
            std::cout << "         - Variante alese: ";
            for (size_t j = 0; j < rez.raspunsuriAlese.size(); ++j) {
                std::cout << rez.raspunsuriAlese[j] + 1;
                if (j + 1 < rez.raspunsuriAlese.size()) std::cout << ", ";
            }
            std::cout << "\n";
        }

        std::cout << "         - Rezultat: " << (rez.corecta ? "Corect" : "Gresit") << "\n";
    }
}

std::ostream& operator<<(std::ostream& out, const Utilizator& u) {
    out << "Utilizator: " << u.nume << " | Scor: " << u.scor;
    return out;
}
//