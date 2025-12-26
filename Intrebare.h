//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_INTREBARE_H
#define OOP_INTREBARE_H

#include <string>
#include <vector>
#include <iostream>

class Intrebare {
protected:
    std::string text;
    std::vector<std::string> variante;

public:
    Intrebare(const std::string& text, const std::vector<std::string>& variante);

    virtual ~Intrebare() = default;

    const std::string& getText() const;
    const std::vector<std::string>& getVariante() const;
    size_t getNrVariante() const;

    //fct virtuala
    virtual bool verificaRaspuns(const std::vector<int>& r) const = 0;
    virtual void afiseaza() const = 0;
};

//derivate
class IntrebareSimpla : public Intrebare {
    int raspunsCorect;

public:
    IntrebareSimpla(const std::string& text, const std::vector<std::string>& variante, int raspunsCorect);
    bool verificaRaspuns(const std::vector<int>& r) const override;
    void afiseaza() const override;
};

class IntrebareMultipla : public Intrebare {
    std::vector<int> raspunsuriCorecte;

public:
    IntrebareMultipla(const std::string& text, const std::vector<std::string>& variante, const std::vector<int>& raspunsuriCorecte);
    bool verificaRaspuns(const std::vector<int>& r) const override;
    void afiseaza() const override;

    // accesor pentru Util/hints etc.
    const std::vector<int>& getRaspunsuriCorecte() const;
};

class IntrebareAdevaratFals : public Intrebare {
    bool raspunsCorect;

public:
    IntrebareAdevaratFals(const std::string& text, bool raspunsCorect);
    bool verificaRaspuns(const std::vector<int>& r) const override;
    void afiseaza() const override;
};

#endif //OOP_INTREBARE_H