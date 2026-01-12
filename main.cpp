#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include "Intrebare.h"
#include "Quiz.h"
#include "Utilizator.h"
#include "JocKahoot.h"
#include "Util.h"
#include "Exceptii.h"

int main() {
    try {
    const std::string caleIntrebari = "intrebari.txt";
    std::ifstream f(caleIntrebari);
    if (!f) {
        throw EroareFisier("Nu s-a putut deschide intrebari.txt (asigura-te ca exista langa executabil)");
    }

    int nrIntrebari;
    f >> nrIntrebari;
    if (!f || nrIntrebari <= 0) {
        throw EroareFormat("intrebari.txt: numar de intrebari invalid");
    }
    f.ignore();

    std::vector<std::unique_ptr<Intrebare>> intrebari;

    for (int i = 0; i < nrIntrebari; ++i) {
        std::string text;
        if (!std::getline(f, text)) throw EroareFormat("intrebari.txt: lipseste textul intrebarii " + std::to_string(i + 1));
        if (!text.empty() && text.back() == '\r') text.pop_back();
        text = trim(text);
        if (text.empty()) { --i; continue; }

        // citeste urmatoarea linie pentru a vedea daca avem un "tipInt" sau direct prima varianta
        std::string linie;
        if (!std::getline(f, linie)) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": lipsesc linii pentru format/variante");
        if (!linie.empty() && linie.back() == '\r') linie.pop_back();
        std::string linieTrim = trim(linie);

        bool aFostTipInt = false;
        int tipInt = -1;
        if (!linieTrim.empty()) {
            std::istringstream iss(linieTrim);
            if ((iss >> tipInt) && iss.eof() && (tipInt == 0 || tipInt == 1 || tipInt == 2 || tipInt == 3)) {
                aFostTipInt = true;
            }
        }

        if (aFostTipInt) {
            // format cu tip de intrebare pe o linie separata
            if (tipInt == 0) {
                std::vector<std::string> variante(4);
                for (int j = 0; j < 4; ++j) {
                    if (!std::getline(f, variante[j])) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": varianta lipsa");
                    if (!variante[j].empty() && variante[j].back() == '\r') variante[j].pop_back();
                }
                int corect = 1;
                f >> corect; // 1-based
                if (!f || corect < 1 || corect > 4) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": index corect invalid (1..4)");
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                intrebari.push_back(std::make_unique<IntrebareSimpla>(text, variante, corect - 1));
            } else if (tipInt == 1) {
                std::vector<std::string> variante(4);
                for (int j = 0; j < 4; ++j) {
                    if (!std::getline(f, variante[j])) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": varianta lipsa");
                    if (!variante[j].empty() && variante[j].back() == '\r') variante[j].pop_back();
                }
                int nrCorecte = 0;
                f >> nrCorecte;
                if (!f || nrCorecte <= 0 || nrCorecte > 4) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": numar raspunsuri corecte invalid");
                std::vector<int> raspCorecte;
                for (int k = 0; k < nrCorecte; ++k) {
                    int idx; f >> idx;
                    if (!f || idx < 1 || idx > 4) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": index corect invalid (1..4)");
                    raspCorecte.push_back(idx - 1);
                }
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                intrebari.push_back(std::make_unique<IntrebareMultipla>(text, variante, raspCorecte));
            } else if (tipInt == 2) {
                int boolAsInt = 0; // 1 pentru Adevarat, 0 pentru Fals
                f >> boolAsInt;
                if (!f || (boolAsInt != 0 && boolAsInt != 1)) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": valoare Adevarat/Fals invalida (0/1)");
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                bool raspCorect = (boolAsInt != 0);
                intrebari.push_back(std::make_unique<IntrebareAdevaratFals>(text, raspCorect));
            } else if (tipInt == 3) {
                // Intrebare cu ordinea raspunsurilor (derivata noua)
                std::vector<std::string> variante(4);
                for (int j = 0; j < 4; ++j) {
                    if (!std::getline(f, variante[j])) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": varianta lipsa");
                    if (!variante[j].empty() && variante[j].back() == '\r') variante[j].pop_back();
                }
                int nrCorecte = 0;
                f >> nrCorecte;
                if (!f || nrCorecte <= 0 || nrCorecte > 4) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": numar raspunsuri corecte invalid");
                std::vector<int> ordineCorecta;
                ordineCorecta.reserve(static_cast<size_t>(nrCorecte));
                for (int k = 0; k < nrCorecte; ++k) {
                    int idx; f >> idx;
                    if (!f || idx < 1 || idx > 4) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": index corect invalid (1..4)");
                    ordineCorecta.push_back(idx - 1); // pastreaza ordinea exact cum e data
                }
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                intrebari.push_back(std::make_unique<IntrebareOrdine>(text, variante, ordineCorecta));
            }
        } else {
            // formatul existent in intrebari.txt: 4 variante urmate de indexul corect
            std::string v1 = linie;
            std::string v2, v3, v4, corectStr;
            if (!std::getline(f, v2)) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": lipseste varianta 2");
            if (!std::getline(f, v3)) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": lipseste varianta 3");
            if (!std::getline(f, v4)) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": lipseste varianta 4");
            if (!std::getline(f, corectStr)) throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": lipseste indicele corect");
            if (!v1.empty() && v1.back() == '\r') v1.pop_back();
            if (!v2.empty() && v2.back() == '\r') v2.pop_back();
            if (!v3.empty() && v3.back() == '\r') v3.pop_back();
            if (!v4.empty() && v4.back() == '\r') v4.pop_back();
            if (!corectStr.empty() && corectStr.back() == '\r') corectStr.pop_back();

            std::vector<std::string> variante = { trim(v1), trim(v2), trim(v3), trim(v4) };
            int corect = 1;
            {
                std::istringstream iss(corectStr);
                if (!(iss >> corect) || corect < 1 || corect > 4) {
                    throw EroareFormat("intrebari.txt: intrebare " + std::to_string(i + 1) + ": index corect invalid (1..4)");
                }
            }
            intrebari.push_back(std::make_unique<IntrebareSimpla>(text, variante, corect - 1));
        }
    }

    // am pus move, fiindca unique_ptr nu poate fi copiat
    Quiz c(std::move(intrebari));

    // Selectie dificultate (0=Toate, 1=Usor, 2=Mediu, 3=Greu)
    int dificultate = selecteazaDificultate();
    Quiz cFiltrat = c.filtreazaDupaDificultate(dificultate);

    if (cFiltrat.size() == 0) {
        throw EroareStareJoc("Nu exista intrebari pentru dificultatea aleasa");
    }

    cFiltrat.amestecaIntrebari();

    size_t totalDisponibile = cFiltrat.size();
    size_t numarSelectat = selecteazaNumarIntrebari(totalDisponibile);
    if (numarSelectat == 0) {
        throw EroareStareJoc("Nu exista suficiente intrebari pentru a porni jocul");
    }

    Quiz chestionarSelectat = cFiltrat.takeFirstNCloned(numarSelectat);

    std::ifstream fin("tastatura.txt");
    if (!fin) {
        throw EroareFisier("Nu s-a putut deschide tastatura.txt");
    }

    int nrJucatori;
    fin >> nrJucatori;
    if (!fin || nrJucatori <= 0) {
        throw EroareFormat("tastatura.txt: numar de jucatori invalid");
    }
    fin.ignore();

    JocKahoot joc(std::move(chestionarSelectat));
    // transmitem meta-datele sesiunii pentru scrierea in leaderboard
    joc.setMetaJoc(dificultate, numarSelectat);
    for (int i = 0; i < nrJucatori; ++i) {
        std::string nume;
        std::getline(fin, nume);
        joc.adaugaUtilizator(Utilizator(nume));
    }

    joc.startJoc(fin);
    std::cout << joc;
    joc.afiseazaRaportGlobal();

    return 0;
    }
    catch (const EroareFisier& e) {
        std::cerr << "[Fisier] " << e.what() << "\n";
        return 2;
    }
    catch (const EroareFormat& e) {
        std::cerr << "[Format] " << e.what() << "\n";
        return 3;
    }
    catch (const EroareStareJoc& e) {
        std::cerr << "[Stare joc] " << e.what() << "\n";
        return 4;
    }
    catch (const EroareAplicatie& e) {
        std::cerr << "[Eroare aplicatie] " << e.what() << "\n";
        return 5;
    }
    catch (const std::exception& e) {
        std::cerr << "[Eroare] " << e.what() << "\n";
        return 1;
    }
}
