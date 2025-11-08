#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iomanip>

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

enum class TipIntrebare { Simpla, Multipla, AdevaratFals };

class Intrebare{

    std::string text;
    std::vector<std::string> variante;
    int raspunsCorect;
    std::vector<int> raspunsuriCorecte;
    TipIntrebare tip;

public:
    //constructor pt intrebarile simple sau adevarat/fals
    Intrebare(const std::string& text, const std::vector<std::string>& variante, int raspunsCorect, TipIntrebare tip = TipIntrebare::Simpla)
        : text(text), variante(variante), tip(tip), raspunsCorect(raspunsCorect) {}

    //constructor pt intrebarile cu raspuns multiplu
    Intrebare(const std::string& text, const std::vector<std::string>& variante, std::vector<int> raspunsuriCorecte)
        : text(text), variante(variante), tip(TipIntrebare::Multipla), raspunsuriCorecte(raspunsuriCorecte) {}

    TipIntrebare getTip() const { return tip; }

    bool verificaRaspuns(const std::vector<int>& r) const {
        if (tip == TipIntrebare::Simpla || tip == TipIntrebare::AdevaratFals) {
            return !r.empty() && r[0] == raspunsCorect;
        }
        else {
            if (r.size() != raspunsuriCorecte.size()) return false;
            std::vector<int> tempR= r;
            std::sort(tempR.begin(), tempR.end());
            std::vector<int> tempCorect = raspunsuriCorecte;
            std::sort(tempCorect.begin(), tempCorect.end());
            return tempR == tempCorect;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Intrebare& i) {
        out << "Intrebare: " << i.text << "\n";
        for (size_t j = 0; j < i.variante.size(); ++j)
            out << "  " << j + 1 << ". " << i.variante[j] << "\n";

        if (i.tip == TipIntrebare::AdevaratFals)    out << "Intrebarea este de tip Adevarat sau Fals\n";
        if (i.tip == TipIntrebare::Multipla)        out<< "Intrebarea are raspunsuri multiple\n";

        return out;
    }
};


class Quiz{

    std::vector<Intrebare> intrebari;

public:
    //constructor cu params
    explicit Quiz(const std::vector<Intrebare>& intrebari = {}) : intrebari(intrebari) {}

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

    //void adaugaIntrebare(const Intrebare& i) { intrebari.push_back(i); }

    const std::vector<Intrebare>& getIntrebari() const { return intrebari; }

    //funcție membru pt amestecarea întrebărilor
    void amestecaIntrebari() {
        static std::random_device rd; // pentru seed
        static std::mt19937 g(rd()); //statice
        std::shuffle(intrebari.begin(), intrebari.end(), g);
    }

    friend std::ostream& operator<<(std::ostream& out, const Quiz& c) {
        out << "Chestionar cu " << c.intrebari.size() << " intrebari:\n";
        for (const auto& i : c.intrebari)
            out << i << "\n";
        return out;
    }
};

class Timer {
    int durata;

public:
    Timer(int secunde = 10) : durata(secunde) {}

    void start() const {
        using namespace std::chrono;
        auto start = steady_clock::now();

        for (int t = durata; t > 0; --t) {
            std::cout << "Timp ramas: " << std::setw(2) << t << " secunde" << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "\r Timpul a expirat!\n";
    }

    bool TimpExpirat(auto startMoment) const {
        using namespace std::chrono;
        auto acum = steady_clock::now();
        auto elapsed = duration_cast<seconds>(acum - startMoment).count();
        return elapsed >= durata;
    }

    int getDurata() const { return durata; }

};

class JocKahoot{

    std::vector<Utilizator> utilizatori;
    Quiz chestionar;

public:
    explicit JocKahoot(const Quiz& c) : chestionar(c) {}

    void adaugaUtilizator(const Utilizator& u) { utilizatori.push_back(u); }

    //fcț membru pentru desfășurarea jocului
    void startJoc(std::istream& in) {
        std::cout << "--- Joc Kahoot ---\n";
        for (auto& user : utilizatori) {
            std::cout << "\n>> Jucator: " << user.getNume() << "\n";
            for (const auto& intrebare : chestionar.getIntrebari()) {
                std::cout << intrebare << "\n";

                Timer t(10);
                auto startMoment = std::chrono::steady_clock::now();

                t.start();

                if (intrebare.getTip() == TipIntrebare::Simpla || intrebare.getTip() == TipIntrebare::AdevaratFals) {
                    int r;
                    std::cout << "Introdu varianta aleasa: ";
                    std::cin >> r;
                    std::cout << "Raspunsul ales: " << r << " -> ";
                    if (intrebare.verificaRaspuns(std::vector<int>{r - 1})) {
                        std::cout << "Corect!\n";
                        user.adaugaScor(10);
                    } else {
                        std::cout << "Gresit!\n";
                    }
                }
                else if (intrebare.getTip() == TipIntrebare::Multipla){
                    int nrRaspunsuri;
                    std::cout << "Cate raspunsuri vrei sa introduci?\n";
                    std::cin >> nrRaspunsuri;
                    std::vector<int> r(nrRaspunsuri);
                    std::cout<< "Introdu raspunsurile tale separate printr-un spatiu\n";
                    for (int j = 0; j < nrRaspunsuri; ++j) { in >> r[j]; }
                    std::cout << "Raspunsurile alese: ";
                    for (auto x : r) std::cout << x << " ";
                    std::cout << "-> ";
                    if (intrebare.verificaRaspuns(r)) {
                        std::cout << "Corect!\n";
                        user.adaugaScor(10);
                    }
                    else {
                        std::cout << "Gresit!\n";
                    }
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
    std::cout << c << std::endl;
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


