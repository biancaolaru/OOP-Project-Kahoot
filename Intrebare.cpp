//
// Created by Bianca on 11/17/2025.
//

#include "Intrebare.h"
#include <algorithm>

Intrebare::Intrebare(const std::string &text, const std::vector<std::string> &variante)
         : text(text), variante(variante){}

const std::string& Intrebare::getText() const {
    return text;
}

const std::vector<std::string>& Intrebare::getVariante() const {
    return variante;
}

size_t Intrebare::getNrVariante() const {
    return variante.size();
}

void Intrebare::afiseaza() const {
    afiseazaImpl();
}

IntrebareSimpla::IntrebareSimpla(const std::string& text, const std::vector<std::string>& variante, int raspunsCorect)
    : Intrebare(text, variante), raspunsCorect(raspunsCorect) {}

bool IntrebareSimpla::verificaRaspuns(const std::vector<int>& r) const {
    return r.size() == 1 && r[0] == raspunsCorect;
}

void IntrebareSimpla::afiseazaImpl() const {
    std::cout << text << "\n";
    for (size_t i = 0; i < variante.size(); ++i)
        std::cout << (i + 1) << ": " << variante[i] << "\n";
}

std::unique_ptr<Intrebare> IntrebareSimpla::clone() const {
    return std::make_unique<IntrebareSimpla>(*this);
}

IntrebareMultipla::IntrebareMultipla(const std::string& text, const std::vector<std::string>& variante, const std::vector<int>& raspunsuriCorecte)
    : Intrebare(text, variante), raspunsuriCorecte(raspunsuriCorecte) {}

bool IntrebareMultipla::verificaRaspuns(const std::vector<int>& r) const {
    if (r.size() != raspunsuriCorecte.size()) return false;
    std::vector<int> a = r;
    std::vector<int> b = raspunsuriCorecte;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

const std::vector<int>& IntrebareMultipla::getRaspunsuriCorecte() const {
    return raspunsuriCorecte;
}

void IntrebareMultipla::afiseazaImpl() const {
    std::cout << text << "\n";
    for (size_t i = 0; i < variante.size(); ++i)
        std::cout << (i + 1) << ": " << variante[i] << "\n";
}

std::unique_ptr<Intrebare> IntrebareMultipla::clone() const {
    return std::make_unique<IntrebareMultipla>(*this);
}

IntrebareAdevaratFals::IntrebareAdevaratFals(const std::string& text, bool raspunsCorect)
    : Intrebare(text, { "Adevarat", "Fals" }), raspunsCorect(raspunsCorect) {}

bool IntrebareAdevaratFals::verificaRaspuns(const std::vector<int>& r) const {
    // raspunsurile sunt zero-based (0 = Adevarat, 1 = Fals)
    return r.size() == 1 && r[0] == (raspunsCorect ? 0 : 1);
}

void IntrebareAdevaratFals::afiseazaImpl() const {
    std::cout << text << "\n";
    std::cout << "1: Adevarat\n2: Fals\n";
}

std::unique_ptr<Intrebare> IntrebareAdevaratFals::clone() const {
    return std::make_unique<IntrebareAdevaratFals>(*this);
}
