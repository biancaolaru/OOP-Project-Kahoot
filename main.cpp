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
#include <mutex>
#include <cctype>

#if defined(__unix__) || defined(__APPLE__)
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#define HAS_POSIX_TERMINAL 1
#else
#define HAS_POSIX_TERMINAL 0
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

namespace {

std::mutex& consoleMutex() {
    static std::mutex m;
    return m;
}

bool supportsAnsiCursorControl() {
    static const bool supported = []() {
#if HAS_POSIX_TERMINAL
        return true;
#elif defined(_WIN32)
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (handle == INVALID_HANDLE_VALUE || handle == nullptr) return false;
        DWORD mode = 0;
        if (!GetConsoleMode(handle, &mode)) return false;
        if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) return true;
        DWORD newMode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(handle, newMode)) return false;
        return true;
#else
        return false;
#endif
    }();
    return supported;
}

std::optional<std::vector<int>> parseAnswerIndices(const std::string& raw, size_t maxOptions) {
    std::istringstream iss(raw);
    std::vector<int> indices;
    int value;

    while (iss >> value) {
        if (value < 1 || value > static_cast<int>(maxOptions)) {
            return std::nullopt;
        }
        indices.push_back(value - 1);
    }

    if (indices.empty()) return std::nullopt;
    if (iss.fail() && !iss.eof()) return std::nullopt;
    return indices;
}

#if HAS_POSIX_TERMINAL
class TerminalModeGuard {
    termios initial{};
    bool active = false;

public:
    TerminalModeGuard() {
        if (!::isatty(STDIN_FILENO)) return;
        if (tcgetattr(STDIN_FILENO, &initial) != 0) return;
        termios raw = initial;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) return;
        active = true;
    }

    ~TerminalModeGuard() {
        if (active) {
            tcsetattr(STDIN_FILENO, TCSANOW, &initial);
        }
    }

    bool isActive() const { return active; }
};
#endif

}

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
    size_t getNrVariante() const { return variante.size(); }

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
    static void updateTimerLine(const std::string& text) {
        std::lock_guard<std::mutex> lock(consoleMutex());
        if (!supportsAnsiCursorControl()) {
            if (text.find_first_not_of(' ') == std::string::npos) return;
            std::cout << "\n" << text << std::flush;
            return;
        }
        std::cout << "\033[s";               // save cursor position
        std::cout << "\033[1A";              // move to timer line
        std::cout << "\r" << text;           // overwrite timer line
        std::cout << "\033[K";               // clear to end of line
        std::cout << "\033[u" << std::flush; // restore cursor
    }

    static std::string formatCountdown(int seconds) {
        std::ostringstream oss;
        oss << "Timp ramas: " << std::setw(2) << seconds << "s";
        return oss.str();
    }

public:
    Timer(int secunde = 10) : durata(secunde) {}

    std::thread startCountdown(std::atomic<bool>& stopRequested, std::atomic<bool>& timeExpired) const {
        using namespace std::chrono_literals;
        return std::thread([this, &stopRequested, &timeExpired]() {
            for (int elapsed = 1; elapsed <= durata; ++elapsed) {
                std::this_thread::sleep_for(1s);
                if (stopRequested.load()) {
                    updateTimerLine(std::string(30, ' '));
                    return;
                }

                int remaining = durata - elapsed;
                if (remaining >= 0) {
                    updateTimerLine(formatCountdown(remaining));
                }
            }

            timeExpired.store(true);
            updateTimerLine("Timpul a expirat!");
        });
    }

    int getDurata() const { return durata; }

};

namespace {

struct CapturedAnswer {
    bool endOfInput = false;
    bool timedOut = false;
    std::optional<std::string> raw;
};

#if HAS_POSIX_TERMINAL
std::optional<std::string> readFromTerminal(std::atomic<bool>& stopTimer,
                                            std::atomic<bool>& timeExpired,
                                            bool& inputEnded) {
    TerminalModeGuard guard;
    if (!guard.isActive()) {
        std::string fallback;
        if (std::getline(std::cin, fallback)) {
            if (!fallback.empty() && fallback.back() == '\r') fallback.pop_back();
            stopTimer.store(true);
            return fallback;
        }
        inputEnded = true;
        return std::nullopt;
    }

    std::string buffer;
    using namespace std::chrono_literals;

    while (!timeExpired.load()) {
        char ch = 0;
        ssize_t readBytes = ::read(STDIN_FILENO, &ch, 1);
        if (readBytes == 1) {
            if (ch == '\n' || ch == '\r') {
                {
                    std::lock_guard<std::mutex> lock(consoleMutex());
                    std::cout << "\n" << std::flush;
                }
                stopTimer.store(true);
                return buffer;
            }
            if (ch == 4) { // Ctrl + D
                inputEnded = true;
                return std::nullopt;
            }
            if (ch == 3) { // Ctrl + C
                inputEnded = true;
                return std::nullopt;
            }
            if (ch == 127 || ch == 8) {
                if (!buffer.empty()) {
                    buffer.pop_back();
                    std::lock_guard<std::mutex> lock(consoleMutex());
                    std::cout << "\b \b" << std::flush;
                }
                continue;
            }
            if (std::isprint(static_cast<unsigned char>(ch))) {
                buffer.push_back(ch);
                std::lock_guard<std::mutex> lock(consoleMutex());
                std::cout << ch << std::flush;
            }
        }
        else if (readBytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(10ms);
                continue;
            }
            if (errno == EINTR) continue;
            break;
        }
        else {
            std::this_thread::sleep_for(10ms);
        }
    }

    if (guard.isActive()) {
        tcflush(STDIN_FILENO, TCIFLUSH);
    }
    return std::nullopt;
}
#endif

CapturedAnswer captureAnswer(std::istream& in,
                             const Timer& t) {
    CapturedAnswer result;
    std::atomic<bool> stopTimer(false);
    std::atomic<bool> timeExpired(false);
    auto timerThread = t.startCountdown(stopTimer, timeExpired);

#if HAS_POSIX_TERMINAL
    const bool interactiveTerminal = (&in == &std::cin) && ::isatty(STDIN_FILENO);
#else
    const bool interactiveTerminal = false;
#endif

    if (interactiveTerminal) {
#if HAS_POSIX_TERMINAL
        result.raw = readFromTerminal(stopTimer, timeExpired, result.endOfInput);
#endif
    }
    else {
        std::string line;
        if (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            result.raw = std::move(line);
            stopTimer.store(true);
        }
        else {
            result.endOfInput = true;
        }
    }

    stopTimer.store(true);
    if (timerThread.joinable()) timerThread.join();

    if (!result.raw.has_value() && timeExpired.load()) {
        result.timedOut = true;
    }

    return result;
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
        for (auto& user : utilizatori) {
            std::cout << "\n>> Jucator: " << user.getNume() << "\n";
            for (const auto& intrebare : chestionar.getIntrebari()) {
                std::cout << intrebare << "\n";
                Timer t(10);

                if (intrebare.getTip() == TipIntrebare::Multipla) {
                    std::cout << "Introdu toate variantele corecte separate prin spatiu.\n";
                }
                else {
                    std::cout << "Introdu o singura varianta (1-" << intrebare.getNrVariante() << ").\n";
                }

                {
                    std::lock_guard<std::mutex> lock(consoleMutex());
                    std::cout << "Timp ramas: " << std::setw(2) << t.getDurata() << "s" << std::endl;
                    std::cout << "> " << std::flush;
                }

                auto raspunsCapturat = captureAnswer(in, t);

                if (raspunsCapturat.endOfInput) {
                    std::cout << "\nNu mai exista raspunsuri de citit. Jocul se incheie.\n";
                    return;
                }

                if (raspunsCapturat.timedOut) {
                    std::cout << "\nTimpul a expirat! Se trece la urmatoarea intrebare.\n";
                    continue;
                }

                if (!raspunsCapturat.raw || raspunsCapturat.raw->empty()) {
                    std::cout << "Nu s-a introdus niciun raspuns valid.\n";
                    continue;
                }

                auto raspunsuri = parseAnswerIndices(*raspunsCapturat.raw, intrebare.getNrVariante());
                if (!raspunsuri) {
                    std::cout << "Raspuns invalid. Tasteaza doar numere separate prin spatii.\n";
                    continue;
                }

                if ((intrebare.getTip() == TipIntrebare::Simpla || intrebare.getTip() == TipIntrebare::AdevaratFals) && raspunsuri->size() != 1) {
                    std::cout << "Alege o singura varianta pentru acest tip de intrebare.\n";
                    continue;
                }

                std::vector<int> raspunsEvaluat = (intrebare.getTip() == TipIntrebare::Multipla)
                    ? *raspunsuri
                    : std::vector<int>{raspunsuri->front()};

                std::cout << "Raspunsul ales: ";
                for (int idx : raspunsEvaluat) {
                    std::cout << idx + 1 << " ";
                }
                std::cout << "-> ";

                if (intrebare.verificaRaspuns(raspunsEvaluat)) {
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

    joc.startJoc();
    std::cout << joc;

    return 0;
}
