//
// Created by Bianca on 11/17/2025.
//

#include "JocKahoot.h"
#include <random>
#include <iomanip>
#include <chrono>
#include <limits>
#include "Util.h"

JocKahoot::JocKahoot(const Quiz& c) : chestionar(c) {}

void JocKahoot::adaugaUtilizator(const Utilizator& u) {
    utilizatori.push_back(u);
}

void JocKahoot::startJoc(std::istream& in) {
    std::cout << "--- Joc Kahoot ---\n";
    std::random_device rd;
    std::mt19937 rng(rd());

    bool fluxTerminat = false;
    bool jocOprit = false;

    for (auto& user : utilizatori) {
        std::cout << "\n>> Jucator: " << user.getNume() << "\n";
        for (const auto& intrebare : chestionar.getIntrebari()) {
            std::cout << intrebare << "\n";
            std::cout << "[H - ajutor | SKIP - sari intrebarea | STOP - opreste jocul]\n";

            RezultatIntrebare rezultat;
            rezultat.intrebare = intrebare.getText();

            std::vector<int> raspunsEvaluat;
            bool finalizat = false;
            while (!finalizat) {
                std::cout << "> " << std::flush;
                std::string linie;
                if (!std::getline(in, linie)) {
                    fluxTerminat = true;
                    break;
                }
                if (!linie.empty() && linie.back() == '\r') linie.pop_back();
                linie = trim(linie);
                if (linie.empty()) {
                    std::cout << "Introdu un raspuns sau o comanda valida.\n";
                    continue;
                }

                const auto comanda = toUpper(linie);
                if (comanda == "STOP") {
                    jocOprit = true;
                    finalizat = true;
                    break;
                }

                if (comanda == "H") {
                    if (user.poateFolosiAjutor()) {
                        afiseazaHint(intrebare, rng);
                        user.consumaAjutor();
                    } else {
                        std::cout << "Ai folosit deja ajutorul unic disponibil pentru aceasta sesiune.\n";
                    }
                    continue;
                }

                if (comanda == "SKIP") {
                    rezultat.sarita = true;
                    finalizat = true;
                    break;
                }

                auto raspunsuri = parseAnswerIndices(linie, intrebare.getNrVariante());
                if (!raspunsuri) {
                    std::cout << "Raspuns invalid. Foloseste numere separate prin spatiu.\n";
                    continue;
                }

                if ((intrebare.getTip() == TipIntrebare::Simpla || intrebare.getTip() == TipIntrebare::AdevaratFals) && raspunsuri->size() != 1) {
                    std::cout << "Pentru acest tip de intrebare trebuie sa alegi o singura varianta.\n";
                    continue;
                }

                raspunsEvaluat = *raspunsuri;
                rezultat.raspunsuriAlese = raspunsEvaluat;
                finalizat = true;
            }

            if (fluxTerminat || jocOprit) break;

            if (rezultat.sarita) {
                std::cout << "Intrebarea a fost sarita.\n";
                user.adaugaRezultat(rezultat);
                continue;
            }

            const bool corect = !raspunsEvaluat.empty() && intrebare.verificaRaspuns(raspunsEvaluat);
            rezultat.corecta = corect;
            user.adaugaRezultat(rezultat);

            std::cout << (corect ? "Corect!\n" : "Gresit!\n");
            if (corect) user.adaugaScor(10);
        }

        if (fluxTerminat) {
            std::cout << "\nNu mai exista raspunsuri disponibile. Jocul se incheie.\n";
            break;
        }

        std::cout << "Scor final: " << user.getScor() << "\n";
        user.afiseazaIstoric();

        if (jocOprit) {
            std::cout << "\nJocul a fost oprit la cererea unui jucator.\n";
            break;
        }
    }
}

void JocKahoot::afiseazaRaportGlobal() const {
    if (utilizatori.empty()) {
        std::cout << "\nNu exista jucatori pentru a genera un raport global.\n";
        return;
    }

    size_t totalIntrebariParcurse = 0;
    size_t raspunsuriCorecte = 0;
    size_t raspunsuriGresite = 0;
    size_t intrebariSarite = 0;
    size_t hinturiFolosite = 0;
    std::map<std::string, int> intrebariDificile;

    for (const auto& user : utilizatori) {
        hinturiFolosite += static_cast<size_t>(user.getAjutoareFolosite());
        for (const auto& rez : user.getIstoric()) {
            ++totalIntrebariParcurse;
            if (rez.sarita) {
                ++intrebariSarite;
                continue;
            }

            if (rez.corecta) {
                ++raspunsuriCorecte;
            }
            else {
                ++raspunsuriGresite;
                intrebariDificile[rez.intrebare]++;
            }
        }
    }

    const size_t totalIncercate = raspunsuriCorecte + raspunsuriGresite;
    const double acuratete = totalIncercate == 0 ? 0.0
        : (static_cast<double>(raspunsuriCorecte) / static_cast<double>(totalIncercate)) * 100.0;

    std::cout << "\n=== Raport global Kahoot ===\n";
    std::cout << "Intrebari parcurse: " << totalIntrebariParcurse << "\n";
    std::cout << " - Corecte: " << raspunsuriCorecte << "\n";
    std::cout << " - Gresite: " << raspunsuriGresite << "\n";
    std::cout << " - Sarite: " << intrebariSarite << "\n";
    std::cout << "Hinturi folosite: " << hinturiFolosite << "\n";
    std::cout << "Acuratete globala: " << std::fixed << std::setprecision(2) << acuratete << "%\n";

    if (!intrebariDificile.empty()) {
        std::vector<std::pair<std::string, int>> ordonate(intrebariDificile.begin(), intrebariDificile.end());
        std::sort(ordonate.begin(), ordonate.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.second == rhs.second) return lhs.first < rhs.first;
            return lhs.second > rhs.second;
        });

        std::cout << "Top intrebari dificile:\n";
        const size_t maximAfisat = std::min<size_t>(3, ordonate.size());
        for (size_t i = 0; i < maximAfisat; ++i) {
            std::cout << " - \"" << ordonate[i].first << "\" a avut "
                      << ordonate[i].second << " raspunsuri gresite\n";
        }
    } else {
        std::cout << "Nicio intrebare nu a fost identificata ca dificila in aceasta sesiune.\n";
    }
}

std::ostream& operator<<(std::ostream& out, const JocKahoot& j) {
    out << "\n--- Clasament final ---\n";
    auto clasament = j.utilizatori;
    std::sort(clasament.begin(), clasament.end(), [](const Utilizator& lhs, const Utilizator& rhs) {
        if (lhs.getScor() == rhs.getScor()) return lhs.getNume() < rhs.getNume();
        return lhs.getScor() > rhs.getScor();
    });

    for (size_t poz = 0; poz < clasament.size(); ++poz) {
        out << poz + 1 << ". " << clasament[poz] << "\n";
    }
    return out;
}
