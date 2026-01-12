//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_INTREBARE_H
#define OOP_INTREBARE_H

#include <string>
#include <vector>
#include <iostream>
#include <memory>

class Intrebare {
protected:
    std::string text;
    std::vector<std::string> variante;

    virtual void afiseazaImpl() const = 0;

public:
    Intrebare(const std::string& text, const std::vector<std::string>& variante);

    virtual ~Intrebare() = default;

    const std::string& getText() const;
    const std::vector<std::string>& getVariante() const;

    // fct virtuala specifica temei
    virtual bool verificaRaspuns(const std::vector<int>& r) const = 0;

    void afiseaza() const;

    // constructor virtual
    virtual std::unique_ptr<Intrebare> clone() const = 0;
};

//derivate
class IntrebareSimpla : public Intrebare {
    int raspunsCorect;

public:
    IntrebareSimpla(const std::string& text, const std::vector<std::string>& variante, int raspunsCorect);
    bool verificaRaspuns(const std::vector<int>& r) const override;
    std::unique_ptr<Intrebare> clone() const override;

protected:
    void afiseazaImpl() const override;
};

class IntrebareMultipla : public Intrebare {
    std::vector<int> raspunsuriCorecte;

public:
    IntrebareMultipla(const std::string& text, const std::vector<std::string>& variante, const std::vector<int>& raspunsuriCorecte);
    bool verificaRaspuns(const std::vector<int>& r) const override;
    std::unique_ptr<Intrebare> clone() const override;

    const std::vector<int>& getRaspunsuriCorecte() const;

protected:
    void afiseazaImpl() const override;
};

class IntrebareAdevaratFals : public Intrebare {
    bool raspunsCorect;

public:
    IntrebareAdevaratFals(const std::string& text, bool raspunsCorect);
    bool verificaRaspuns(const std::vector<int>& r) const override;
    std::unique_ptr<Intrebare> clone() const override;

protected:
    void afiseazaImpl() const override;
};

// noua derivata: ordinea raspunsurilor conteaza (mosteneste din IntrebareMultipla)
class IntrebareOrdine : public IntrebareMultipla {
public:
    IntrebareOrdine(const std::string& text, const std::vector<std::string>& variante, const std::vector<int>& ordineCorecta)
        : IntrebareMultipla(text, variante, ordineCorecta) {}

    bool verificaRaspuns(const std::vector<int>& r) const override;
    std::unique_ptr<Intrebare> clone() const override;

protected:
    void afiseazaImpl() const override;
};

// operator<<
inline std::ostream& operator<<(std::ostream& out, const Intrebare& q) {
    q.afiseaza();
    return out;
}

#endif //OOP_INTREBARE_H