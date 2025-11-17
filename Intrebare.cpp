//
// Created by Bianca on 11/17/2025.
//

#include "Intrebare.h"
#include <algorithm>

Intrebare::Intrebare(const std::string& text,
                     const std::vector<std::string>& variante,
                     int raspunsCorect,
                     TipIntrebare tip)
    : text(text), variante(variante), raspunsCorect(raspunsCorect), tip(tip) {}

Intrebare::Intrebare(const std::string& text,
                     const std::vector<std::string>& variante,
                     std::vector<int> raspunsuriCorecte)
    : text(text), variante(variante),
      raspunsuriCorecte(raspunsuriCorecte),
      tip(TipIntrebare::Multipla) {}

TipIntrebare Intrebare::getTip() const { return tip; }
const std::string& Intrebare::getText() const { return text; }
const std::vector<std::string>& Intrebare::getVariante() const { return variante; }
size_t Intrebare::getNrVariante() const { return variante.size(); }

std::vector<int> Intrebare::getRaspunsuriCorecte() const {
    if (tip == TipIntrebare::Multipla) return raspunsuriCorecte;
    return { raspunsCorect };
}

bool Intrebare::verificaRaspuns(const std::vector<int>& r) const {
    if (tip != TipIntrebare::Multipla) {
        return !r.empty() && r[0] == raspunsCorect;
    }

    if (r.size() != raspunsuriCorecte.size()) return false;

    auto a = r;
    auto b = raspunsuriCorecte;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());

    return a == b;
}

std::ostream& operator<<(std::ostream& out, const Intrebare& i) {
    out << "Intrebare: " << i.text << "\n";
    for (size_t j = 0; j < i.variante.size(); ++j)
        out << "  " << j + 1 << ". " << i.variante[j] << "\n";

    if (i.tip == TipIntrebare::AdevaratFals)
        out << "Intrebarea este de tip Adevarat/Fals\n";

    if (i.tip == TipIntrebare::Multipla)
        out << "Intrebarea are raspunsuri multiple\n";

    return out;
}
