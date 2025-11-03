#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

class Utilizator{

    std::string nume;
    int scor;

public:
    Utilizator(const std::string& nume = "Anonim", int scor = 0) : nume(nume), scor(scor) {}

    void adaugaScor(int puncte) { scor += puncte; }
    int getScor() const { return scor; }
    std::string getNume() const { return nume; }

    friend std::ostream& operator<<(std::ostream& out, const Utilizator& u) {
        out << "Utilizator: " << u.nume << " | Scor: " << u.scor;
        return out;
    }
};

class Intrebare{

    std::string text;
    std::vector<std::string> variante;
    int raspunsCorect;

public:
    Intrebare(const std::string& text = "", const std::vector<std::string>& variante = {}, int raspunsCorect = 0)
        : text(text), variante(variante), raspunsCorect(raspunsCorect) {}

    bool verificaRaspuns(int r) const { return r == raspunsCorect; }

    friend std::ostream& operator<<(std::ostream& out, const Intrebare& i) {
        out << "Intrebare: " << i.text << "\n";
        for (size_t j = 0; j < i.variante.size(); ++j)
            out << "  " << j + 1 << ". " << i.variante[j] << "\n";
        return out;
    }
};


class Quiz{

    std::vector<Intrebare> intrebari;

public:
    //constructor cu params
    Quiz(const std::vector<Intrebare>& intrebari = {}) : intrebari(intrebari) {}

    //constructor de copiere
    Quiz(const Quiz& other) {
        intrebari = other.intrebari;
    }

    //operator= de copiere
    Quiz& operator=(const Quiz& other) {
        if (this != &other)
            intrebari = other.intrebari;
        return *this;
    }

    //destructor
    ~Quiz() = default;

    void adaugaIntrebare(const Intrebare& i) { intrebari.push_back(i); }

    const std::vector<Intrebare>& getIntrebari() const { return intrebari; }

    //funcție membru pt amestecarea întrebărilor
    void amestecaIntrebari() {
        std::random_shuffle(intrebari.begin(), intrebari.end());
    }

    friend std::ostream& operator<<(std::ostream& out, const Quiz& c) {
        out << "Chestionar cu " << c.intrebari.size() << " întrebări:\n";
        for (const auto& i : c.intrebari)
            out << i << "\n";
        return out;
    }
};


class JocKahoot{

    std::vector<Utilizator> utilizatori;
    Quiz chestionar;

public:
    JocKahoot(const Quiz& c) : chestionar(c) {}

    void adaugaUtilizator(const Utilizator& u) { utilizatori.push_back(u); }

    //fcț membru pentru desfășurarea jocului
    void startJoc(std::istream& in) {
        std::cout << "--- Joc Kahoot ---\n";
        for (auto& user : utilizatori) {
            std::cout << "\n>> Jucator: " << user.getNume() << "\n";
            for (const auto& intrebare : chestionar.getIntrebari()) {
                std::cout << intrebare;
                int r;
                in >> r;
                std::cout << "Raspunsul ales: " << r << " -> ";
                if (intrebare.verificaRaspuns(r - 1)) {
                    std::cout << "Corect!\n";
                    user.adaugaScor(10);
                } else {
                    std::cout << "Gresit!\n";
                }
            }
            std::cout << "Scor final: " << user.getScor() << "\n";
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const JocKahoot& j) {
        out << "\n--- Clasament final ---\n";
        for (const auto& u : j.utilizatori)
            out << u << "\n";
        return out;
    }
};


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
    c.amestecaIntrebari();


    std::ifstream fin("tastatura.txt");
    if (!fin) {
        std::cerr << "Eroare: nu s-a putut deschide tastatura.txt\n";
        return 1;
    }

    int nrJucatori;
    fin >> nrJucatori;
    fin.ignore();

    JocKahoot joc(c);

    for (int i = 0; i < nrJucatori; ++i) {
        std::string nume;
        std::getline(fin, nume);
        joc.adaugaUtilizator(Utilizator(nume));
    }

    joc.startJoc(fin);
    std::cout << joc;

    return 0;
}


