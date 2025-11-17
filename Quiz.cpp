//
// Created by Bianca on 11/17/2025.
//

#include "Quiz.h"
#include <algorithm>
#include <random>

Quiz::Quiz(const std::vector<Intrebare>& intrebari)
    : intrebari(intrebari) {}

Quiz::Quiz(const Quiz& other)
    : intrebari(other.intrebari)
{}

Quiz& Quiz::operator=(const Quiz& other) {
    if (this != &other)
        intrebari = other.intrebari;
    return *this;
}

const std::vector<Intrebare>& Quiz::getIntrebari() const {
    return intrebari;
}

void Quiz::amestecaIntrebari() {
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(intrebari.begin(), intrebari.end(), g);
}

std::ostream& operator<<(std::ostream& out, const Quiz& c) {
    out << "Chestionar cu " << c.intrebari.size() << " intrebari:\n";
    for (const auto& i : c.intrebari)
        out << i << "\n";
    return out;
}