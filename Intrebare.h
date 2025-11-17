//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_INTREBARE_H
#define OOP_INTREBARE_H

#include <string>
#include <vector>
#include <iostream>

enum class TipIntrebare { Simpla, Multipla, AdevaratFals };

class Intrebare {

    std::string text;
    std::vector<std::string> variante;
    int raspunsCorect;
    std::vector<int> raspunsuriCorecte;
    TipIntrebare tip;

public:
    Intrebare(const std::string& text,
              const std::vector<std::string>& variante,
              int raspunsCorect,
              TipIntrebare tip = TipIntrebare::Simpla);

    Intrebare(const std::string& text,
              const std::vector<std::string>& variante,
              std::vector<int> raspunsuriCorecte);

    TipIntrebare getTip() const;
    const std::string& getText() const;
    const std::vector<std::string>& getVariante() const;
    size_t getNrVariante() const;

    std::vector<int> getRaspunsuriCorecte() const;

    bool verificaRaspuns(const std::vector<int>& r) const;

    friend std::ostream& operator<<(std::ostream& out, const Intrebare& i);
};

#endif //OOP_INTREBARE_H