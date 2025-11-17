#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Intrebare.h"
#include "Quiz.h"
#include "Utilizator.h"
#include "JocKahoot.h"
#include "Util.h"

int main() {
    std::ifstream f("intrebari.txt");
    if (!f) {
        std::cerr << "Eroare: nu s-a putut deschide intrebari.txt\n";
        return 1;
    }

    int nrIntrebari;
    f >> nrIntrebari;
    f.ignore();

    std::vector<Intrebare> intrebari;
    for (int i = 0; i < nrIntrebari; ++i) {
        std::string text;
        std::getline(f, text);
        std::vector<std::string> variante(4);
        for (int j = 0; j < 4; ++j)
            std::getline(f, variante[j]);
        int corect;
        f >> corect;
        f.ignore();
        intrebari.emplace_back(text, variante, corect - 1);
    }

    Quiz c(intrebari);
    std::cout << c << std::endl;
    c.amestecaIntrebari();

    size_t totalDisponibile = c.getIntrebari().size();
    size_t numarSelectat = selecteazaNumarIntrebari(totalDisponibile);
    if (numarSelectat == 0) {
        std::cerr << "Nu exista suficiente intrebari pentru a porni jocul.\n";
        return 1;
    }

    auto inceput = c.getIntrebari().begin();
    auto sfarsit = inceput + static_cast<long>(numarSelectat);
    std::vector<Intrebare> intrebariSelectate(inceput, sfarsit);
    Quiz chestionarSelectat(intrebariSelectate);

    std::ifstream fin("tastatura.txt");
    if (!fin) {
        std::cerr << "Eroare: nu s-a putut deschide tastatura.txt\n";
        return 1;
    }

    int nrJucatori;
    fin >> nrJucatori;
    fin.ignore();

    JocKahoot joc(chestionarSelectat);
    for (int i = 0; i < nrJucatori; ++i) {
        std::string nume;
        std::getline(fin, nume);
        joc.adaugaUtilizator(Utilizator(nume));
    }

    joc.startJoc();
    std::cout << joc;
    joc.afiseazaRaportGlobal();

    return 0;
}
