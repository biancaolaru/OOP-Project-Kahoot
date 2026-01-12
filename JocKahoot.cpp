//
// Created by Bianca on 11/17/2025.
//

#include "JocKahoot.h"
#include <random>
#include <iomanip>
#include <limits>
#include <numeric>
#include "Util.h"


JocKahoot::JocKahoot(Quiz&& c) : chestionar(std::move(c)) {}

void JocKahoot::adaugaUtilizator(const Utilizator& u) {
    utilizatori.push_back(u);
}

void JocKahoot::startJoc(std::istream& in) {
    std::cout << "--- Joc Kahoot ---\n";
    std::random_device rd;
    std::mt19937 rng(rd());

    bool fluxTerminat = false;
    bool jocOprit = false;

    // poate comuta de la fisier la tastatura o singura data
    std::istream* currentIn = &in;
    bool aComutatLaTastatura = false;
    bool aCititDinFisier = false;

    //de revizuit aici si eventual spart in mai multe functii(?)
    for (auto& user : utilizatori) {
        std::cout << "\n>> Jucator: " << user.getNume() << "\n";
        for (const auto& intrebare : chestionar.getIntrebari()) {
            // generam permutarea variantelor pentru acest jucator si aceasta intrebare
            const auto& variante = intrebare->getVariante();
            const size_t nOpt = variante.size();
            std::vector<int> perm(nOpt);
            std::iota(perm.begin(), perm.end(), 0);
            if (nOpt >= 2) std::shuffle(perm.begin(), perm.end(), rng);

            // afisam intrebarea si variantele in ordinea permutata
            std::cout << intrebare->getText() << "\n";
            for (size_t i = 0; i < nOpt; ++i) {
                int origIdx = perm[i];
                std::cout << (i + 1) << ": " << variante[static_cast<size_t>(origIdx)] << "\n";
            }
            std::cout << "[H - ajutor | SKIP - sari intrebarea | STOP - opreste jocul]\n";

            RezultatIntrebare rezultat;
            rezultat.intrebare = intrebare->getText();

            std::vector<int> raspunsEvaluat;
            bool finalizat = false;
            while (!finalizat) {
                std::cout << "> " << std::flush;
                std::string linie;
                bool citeaDinFisier = (currentIn != &std::cin);
                if (!std::getline(*currentIn, linie)) {
                    // daca am epuizat fisierul/scriptul, comuta o singura data la tastatura
                    if (currentIn != &std::cin) {
                        if (!aComutatLaTastatura) {
                            if (aCititDinFisier) {
                                std::cout << "\n[Intrari epuizate in fisier. Continua de la tastatura...]\n";
                            }
                            aComutatLaTastatura = true;
                        }
                        currentIn = &std::cin;
                        // incearca din nou sa citesti de la tastatura in aceeasi iteratie
                        continue;
                    } else {
                        fluxTerminat = true;
                        break;
                    }
                }
                if (citeaDinFisier) aCititDinFisier = true;
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
                        afiseazaHint(intrebare, rng, &perm);
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

                auto raspunsuriDisplay = parseAnswerIndices(linie, nOpt);
                if (!raspunsuriDisplay) {
                    std::cout << "Raspuns invalid. Foloseste numere separate prin spatiu.\n";
                    continue;
                }

                // pt IntrebareSimpla si AdevaratFals, se accepta un singur raspuns
                const bool eMultipla = (dynamic_cast<IntrebareMultipla*>(intrebare.get()) != nullptr);
                if (!eMultipla && raspunsuriDisplay->size() != 1) {
                    std::cout << "Pentru acest tip de intrebare trebuie sa alegi o singura varianta.\n";
                    continue;
                }

                // mapeaza din indicii afisati (display) in indicii canonici
                raspunsEvaluat.clear();
                raspunsEvaluat.reserve(raspunsuriDisplay->size());
                for (int dispIdx0 : *raspunsuriDisplay) {
                    if (dispIdx0 < 0 || static_cast<size_t>(dispIdx0) >= nOpt) { raspunsEvaluat.clear(); break; }
                    int origIdx = perm[static_cast<size_t>(dispIdx0)];
                    raspunsEvaluat.push_back(origIdx);
                }
                if (raspunsEvaluat.empty() && !raspunsuriDisplay->empty()) {
                    std::cout << "Raspuns invalid. Indici in afara intervalului.\n";
                    continue;
                }

                rezultat.raspunsuriAlese = raspunsEvaluat;
                finalizat = true;
            }

            if (fluxTerminat || jocOprit) break;

            if (rezultat.sarita) {
                std::cout << "Intrebarea a fost sarita.\n";
                // resetam streak-ul la intrebare sarita
                user.inregistreazaRaspuns(false, true);
                user.adaugaRezultat(rezultat);
                continue;
            }

            const bool corect = !raspunsEvaluat.empty() && intrebare->verificaRaspuns(raspunsEvaluat);
            rezultat.corecta = corect;
            user.adaugaRezultat(rezultat);

            // Scorare: 100p baza; intrebari multiple si de ordine au punctaj dublu (200p)
            const bool esteMultiplaSauOrdine = (dynamic_cast<IntrebareMultipla*>(intrebare.get()) != nullptr);
            const int baza = 100;
            const int multiplicator = esteMultiplaSauOrdine ? 2 : 1;

            // actualizam streak-ul si calculam bonusul/scorul doar daca este corect
            user.inregistreazaRaspuns(corect, false);
            if (corect) {
                const int puncteBaza = baza * multiplicator;
                const int bonusStreak = user.calculeazaBonusStreak(multiplicator);
                user.adaugaScor(puncteBaza + bonusStreak);
                if (bonusStreak > 0) {
                    std::cout << "Corect! +" << puncteBaza << "p (+streak " << bonusStreak << "p)";
                } else {
                    std::cout << "Corect! +" << puncteBaza << "p";
                }
            } else {
                std::cout << "Gresit! +0p";
            }
            std::cout << " | Scor curent: " << user.getScor() << "\n";
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

    // Afiseaza leaderboard-ul sesiunii (redenumit simplu "Leaderboard")
    std::cout << "\n--- Leaderboard ---\n";
    auto clasament = utilizatori;
    std::sort(clasament.begin(), clasament.end(), [](const Utilizator& lhs, const Utilizator& rhs) {
        if (lhs.getScor() == rhs.getScor()) return lhs.getNume() < rhs.getNume();
        return lhs.getScor() > rhs.getScor();
    });
    for (size_t poz = 0; poz < clasament.size(); ++poz) {
        std::cout << poz + 1 << ". Utilizator: " << clasament[poz].getNume()
                  << " | Scor: " << clasament[poz].getScor() << "\n";
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
