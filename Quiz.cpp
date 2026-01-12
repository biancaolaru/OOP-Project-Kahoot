//
// Created by Bianca on 11/17/2025.
//

#include "Quiz.h"
#include <algorithm>
#include <random>
#include "Util.h"

size_t Quiz::size() const noexcept {
    return intrebari.size();
}

Quiz Quiz::takeFirstNCloned(size_t n) const {
    const size_t m = std::min(n, intrebari.size());
    std::vector<std::unique_ptr<Intrebare>> vec;
    vec.reserve(m);
    for (size_t i = 0; i < m; ++i) {
        if (intrebari[i]) vec.push_back(intrebari[i]->clone());
    }
    return Quiz(std::move(vec));
}

Quiz Quiz::filtreazaDupaDificultate(int dificultate) const {
    if (dificultate == 0) {
        // toate
        return Quiz(*this);
    }
    std::vector<std::unique_ptr<Intrebare>> vec;
    vec.reserve(intrebari.size());
    for (const auto& q : intrebari) {
        if (!q) continue;
        int d = calculeazaDificultate(q);
        if (d == dificultate) vec.push_back(q->clone());
    }
    return Quiz(std::move(vec));
}

void Quiz::amestecaIntrebari() {
    std::shuffle(intrebari.begin(), intrebari.end(), globalRng());
}
