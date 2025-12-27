#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <limits>
#include "Intrebare.h"
#include "Quiz.h"
#include "Utilizator.h"
#include "JocKahoot.h"
#include "Util.h"

int main() {
    const std::string caleIntrebari = "intrebari.txt";
    std::ifstream f(caleIntrebari);
    if (!f) {
        std::cerr << "Eroare: nu s-a putut deschide intrebari.txt (asigura-te ca exista langa executabil)\n";
        return 1;
    }

    int nrIntrebari;
    f >> nrIntrebari;
    f.ignore();

    std::vector<std::unique_ptr<Intrebare>> intrebari;

    for (int i = 0; i < nrIntrebari; ++i) {
        std::string text;
        if (!std::getline(f, text)) break;
        if (!text.empty() && text.back() == '\r') text.pop_back();
        text = trim(text);
        if (text.empty()) { --i; continue; }

        // Citeste urmatoarea linie pentru a vedea daca avem un "tipInt" sau direct prima varianta
        std::streampos posInainte = f.tellg();
        std::string linie;
        if (!std::getline(f, linie)) break;
        if (!linie.empty() && linie.back() == '\r') linie.pop_back();
        std::string linieTrim = trim(linie);

        bool aFostTipInt = false;
        int tipInt = -1;
        if (!linieTrim.empty()) {
            std::istringstream iss(linieTrim);
            if ((iss >> tipInt) && iss.eof() && (tipInt == 0 || tipInt == 1 || tipInt == 2)) {
                aFostTipInt = true;
            }
        }

        if (aFostTipInt) {
            // Format cu tip de intrebare pe o linie separata
            if (tipInt == 0) {
                std::vector<std::string> variante(4);
                for (int j = 0; j < 4; ++j) {
                    std::getline(f, variante[j]);
                    if (!variante[j].empty() && variante[j].back() == '\r') variante[j].pop_back();
                }
                int corect = 1;
                f >> corect; // 1-based
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                intrebari.push_back(std::make_unique<IntrebareSimpla>(text, variante, corect - 1));
            } else if (tipInt == 1) {
                std::vector<std::string> variante(4);
                for (int j = 0; j < 4; ++j) {
                    std::getline(f, variante[j]);
                    if (!variante[j].empty() && variante[j].back() == '\r') variante[j].pop_back();
                }
                int nrCorecte = 0;
                f >> nrCorecte;
                std::vector<int> raspCorecte;
                for (int k = 0; k < nrCorecte; ++k) {
                    int idx; f >> idx; raspCorecte.push_back(idx - 1);
                }
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                intrebari.push_back(std::make_unique<IntrebareMultipla>(text, variante, raspCorecte));
            } else if (tipInt == 2) {
                int boolAsInt = 0; // 1 pentru Adevarat, 0 pentru Fals
                f >> boolAsInt;
                f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                bool raspCorect = (boolAsInt != 0);
                intrebari.push_back(std::make_unique<IntrebareAdevaratFals>(text, raspCorect));
            }
        } else {
            // Formatul existent in intrebari.txt: 4 variante urmate de indexul corect
            std::string v1 = linie;
            std::string v2, v3, v4, corectStr;
            if (!std::getline(f, v2)) break;
            if (!std::getline(f, v3)) break;
            if (!std::getline(f, v4)) break;
            if (!std::getline(f, corectStr)) break;
            if (!v1.empty() && v1.back() == '\r') v1.pop_back();
            if (!v2.empty() && v2.back() == '\r') v2.pop_back();
            if (!v3.empty() && v3.back() == '\r') v3.pop_back();
            if (!v4.empty() && v4.back() == '\r') v4.pop_back();
            if (!corectStr.empty() && corectStr.back() == '\r') corectStr.pop_back();

            std::vector<std::string> variante = { trim(v1), trim(v2), trim(v3), trim(v4) };
            int corect = 1;
            {
                std::istringstream iss(corectStr);
                if (!(iss >> corect)) corect = 1;
            }
            intrebari.push_back(std::make_unique<IntrebareSimpla>(text, variante, corect - 1));
        }
    }

    // am pus move, fiindca unique_ptr nu poate fi copiat
    Quiz c(std::move(intrebari));
    c.amestecaIntrebari();

    size_t totalDisponibile = c.getIntrebari().size();
    size_t numarSelectat = selecteazaNumarIntrebari(totalDisponibile);
    if (numarSelectat == 0) {
        std::cerr << "Nu exista suficiente intrebari pentru a porni jocul.\n";
        return 1;
    }

    auto inceput = c.getIntrebari().begin();
    auto sfarsit = inceput + static_cast<long>(numarSelectat);
    std::vector<std::unique_ptr<Intrebare>> intrebariSelectate;
    intrebariSelectate.reserve(numarSelectat);
    for (auto it = inceput; it != sfarsit; ++it) {
        if (it->get()) intrebariSelectate.push_back((*it)->clone());
    }

    Quiz chestionarSelectat(std::move(intrebariSelectate));

    std::ifstream fin("tastatura.txt");
    if (!fin) {
        std::cerr << "Eroare: nu s-a putut deschide tastatura.txt\n";
        return 1;
    }

    int nrJucatori;
    fin >> nrJucatori;
    fin.ignore();

    JocKahoot joc(std::move(chestionarSelectat));
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
