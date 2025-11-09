#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <optional>
#include <limits>
#include <cctype>

namespace {

std::string trim(const std::string& value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

std::string toUpper(std::string text) {
    for (auto& ch : text) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return text;
}

std::optional<std::vector<int>> parseAnswerIndices(const std::string& raw, size_t maxOptions) {
    std::istringstream iss(raw);
    std::vector<int> indices;
    int value;
    while (iss >> value) {
        if (value < 1 || value > static_cast<int>(maxOptions)) return std::nullopt;
        indices.push_back(value - 1);
    }
    if (indices.empty()) return std::nullopt;
    if (iss.fail() && !iss.eof()) return std::nullopt;
    return indices;
}

size_t selecteazaNumarIntrebari(size_t totalDisponibile) {
    if (totalDisponibile == 0) return 0;

    std::cout << "\nCate intrebari doresti sa joci (1-" << totalDisponibile << ")? ";
    size_t dorite = totalDisponibile;
    size_t input = 0;
    if (std::cin >> input && input >= 1 && input <= totalDisponibile) {
        dorite = input;
    }
    else {
        std::cout << "Valoare invalida. Se vor folosi toate intrebarile disponibile.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return dorite;
}

}

struct RezultatIntrebare {
    std::string intrebare;
    std::vector<int> raspunsuriAlese;
    bool corecta = false;
    bool sarita = false;
};

class Utilizator{

    std::string nume;
    int scor;
    bool ajutorDisponibil = true;
    std::vector<RezultatIntrebare> istoric;

public:
    Utilizator(const std::string& nume = "Anonim", int scor = 0) : nume(nume), scor(scor) {}

    void adaugaScor(int puncte) { scor += puncte; }
    int getScor() const { return scor; }
    std::string getNume() const { return nume; }
    bool poateFolosiAjutor() const { return ajutorDisponibil; }
    void consumaAjutor() { ajutorDisponibil = false; }
    void adaugaRezultat(const RezultatIntrebare& rez) { istoric.push_back(rez); }

    void afiseazaIstoric() const {
        std::cout << "\n   >> Raport pentru " << nume << " (" << scor << " puncte)\n";
        if (istoric.empty()) {
            std::cout << "      Nu exista intrebari parcurse in aceasta sesiune.\n";
            return;
        }
        for (size_t i = 0; i < istoric.size(); ++i) {
            const auto& rez = istoric[i];
            std::cout << "      " << i + 1 << ". " << rez.intrebare << "\n";
            if (rez.sarita) {
                std::cout << "         - Intrebarea a fost sarita\n";
                continue;
            }
            if (rez.raspunsuriAlese.empty()) {
                std::cout << "         - Nu a fost introdus niciun raspuns\n";
            }
            else {
                std::cout << "         - Variante alese: ";
                for (size_t j = 0; j < rez.raspunsuriAlese.size(); ++j) {
                    std::cout << rez.raspunsuriAlese[j] + 1;
                    if (j + 1 < rez.raspunsuriAlese.size()) std::cout << ", ";
                }
                std::cout << "\n";
            }
            std::cout << "         - Rezultat: " << (rez.corecta ? "Corect" : "Gresit") << "\n";
        }
    }

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
    const std::string& getText() const { return text; }
    const std::vector<std::string>& getVariante() const { return variante; }
    size_t getNrVariante() const { return variante.size(); }
    std::vector<int> getRaspunsuriCorecte() const {
        if (tip == TipIntrebare::Simpla || tip == TipIntrebare::AdevaratFals) return { raspunsCorect };
        return raspunsuriCorecte;
    }

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
        std::cout << "[Cronometru dezactivat temporar pentru a nu bloca introducerea raspunsurilor]\n";
    }

    bool TimpExpirat(const std::chrono::steady_clock::time_point&) const { return false; }

    int getDurata() const { return durata; }

};

void afiseazaHint(const Intrebare& intrebare, std::mt19937& rng) {
    const auto& variante = intrebare.getVariante();
    const auto corecte = intrebare.getRaspunsuriCorecte();

    if (intrebare.getTip() == TipIntrebare::Multipla) {
        std::cout << "Sugestie: Exista " << corecte.size()
                  << " raspunsuri corecte pentru aceasta intrebare.\n";
        return;
    }

    std::uniform_int_distribution<size_t> dist(0, corecte.size() - 1);
    const auto indexSelectat = corecte[dist(rng)];
    if (indexSelectat < variante.size()) {
        std::cout << "Sugestie: Gandeste-te la varianta \"" << variante[indexSelectat] << "\".\n";
    } else {
        std::cout << "Sugestie: Raspunsul corect se afla printre optiunile numerotate mici.\n";
    }
}

class JocKahoot{

    std::vector<Utilizator> utilizatori;
    Quiz chestionar;

public:
    explicit JocKahoot(const Quiz& c) : chestionar(c) {}

    void adaugaUtilizator(const Utilizator& u) { utilizatori.push_back(u); }

    //fcț membru pentru desfășurarea jocului
    void startJoc(std::istream& in = std::cin) {
        std::cout << "--- Joc Kahoot ---\n";
        std::random_device rd;
        std::mt19937 rng(rd());

        bool fluxTerminat = false;
        bool jocOprit = false;

        for (auto& user : utilizatori) {
            std::cout << "\n>> Jucator: " << user.getNume() << "\n";
            for (const auto& intrebare : chestionar.getIntrebari()) {
                std::cout << intrebare << "\n";
                std::cout << "[H - ajutor | SKIP - sari intrebarea | STOP - opreste jocul]\n";

                RezultatIntrebare rezultat;
                rezultat.intrebare = intrebare.getText();

                std::vector<int> raspunsEvaluat;
                bool finalizat = false;
                while (!finalizat) {
                    std::cout << "> " << std::flush;
                    std::string linie;
                    if (!std::getline(in, linie)) {
                        fluxTerminat = true;
                        break;
                    }
                    if (!linie.empty() && linie.back() == '\r') linie.pop_back();
                    linie = trim(linie);
                    if (linie.empty()) {
                        std::cout << "Introdu un raspuns sau o comanda valida.\n";
                        continue;
                    }

                    const auto comanda = toUpper(linie);
                    if (comanda == "STOP") {
                        jocOprit = true;
                        finalizat = true;
                        break;
                    }

                    if (comanda == "H") {
                        if (user.poateFolosiAjutor()) {
                            afiseazaHint(intrebare, rng);
                            user.consumaAjutor();
                        }
                        else {
                            std::cout << "Ai folosit deja ajutorul unic disponibil pentru aceasta sesiune.\n";
                        }
                        continue;
                    }

                    if (comanda == "SKIP") {
                        rezultat.sarita = true;
                        finalizat = true;
                        break;
                    }

                    auto raspunsuri = parseAnswerIndices(linie, intrebare.getNrVariante());
                    if (!raspunsuri) {
                        std::cout << "Raspuns invalid. Foloseste numere separate prin spatiu.\n";
                        continue;
                    }

                    if ((intrebare.getTip() == TipIntrebare::Simpla || intrebare.getTip() == TipIntrebare::AdevaratFals) && raspunsuri->size() != 1) {
                        std::cout << "Pentru acest tip de intrebare trebuie sa alegi o singura varianta.\n";
                        continue;
                    }

                    raspunsEvaluat = *raspunsuri;
                    rezultat.raspunsuriAlese = raspunsEvaluat;
                    finalizat = true;
                }

                if (fluxTerminat || jocOprit) break;

                if (rezultat.sarita) {
                    std::cout << "Intrebarea a fost sarita.\n";
                    user.adaugaRezultat(rezultat);
                    continue;
                }

                const bool corect = !raspunsEvaluat.empty() && intrebare.verificaRaspuns(raspunsEvaluat);
                rezultat.corecta = corect;
                user.adaugaRezultat(rezultat);

                std::cout << (corect ? "Corect!\n" : "Gresit!\n");
                if (corect) user.adaugaScor(10);
            }

            if (fluxTerminat) {
                std::cout << "\nNu mai exista raspunsuri disponibile. Jocul se incheie.\n";
                break;
            }

            std::cout << "Scor final: " << user.getScor() << "\n";
            user.afiseazaIstoric();

            if (jocOprit) {
                std::cout << "\nJocul a fost oprit la cererea unui jucator.\n";
                break;
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const JocKahoot& j) {
        out << "\n--- Clasament final ---\n";
        auto clasament = j.utilizatori;
        std::sort(clasament.begin(), clasament.end(), [](const Utilizator& lhs, const Utilizator& rhs) {
            if (lhs.getScor() == rhs.getScor()) return lhs.getNume() < rhs.getNume();
            return lhs.getScor() > rhs.getScor();
        });

        for (size_t poz = 0; poz < clasament.size(); ++poz) {
            out << poz + 1 << ". " << clasament[poz] << "\n";
        }
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

    return 0;
}
