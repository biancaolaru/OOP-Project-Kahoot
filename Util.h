//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_UTIL_H
#define OOP_UTIL_H

#pragma once
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <limits>
#include <memory>
#include "Intrebare.h"

inline std::string trim(const std::string& value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

inline std::string toUpper(std::string text) {
    for (auto& ch : text) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return text;
}

inline std::optional<std::vector<int>> parseAnswerIndices(const std::string& raw, size_t maxOptions) {
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

inline size_t selecteazaNumarIntrebari(size_t totalDisponibile) {
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

inline void afiseazaHint(const std::unique_ptr<Intrebare>& intrebare, std::mt19937& rng) {
    const auto& variante = intrebare->getVariante();
    const size_t n = variante.size();
    if (n == 0) {
        std::cout << "[Hint indisponibil]\n";
        return;
    }

    // detecteaza tipul intrebarii
    if (auto* mult = dynamic_cast<IntrebareMultipla*>(intrebare.get())) {
        // Multiple choice: arata cate raspunsuri sunt corecte si dezvaluie unul corect aleator
        const auto& corecte = mult->getRaspunsuriCorecte();
        if (corecte.empty()) {
            std::cout << "[Hint] Exista mai multe raspunsuri corecte.\n";
            return;
        }
        std::vector<int> corecteFiltrate;
        corecteFiltrate.reserve(corecte.size());
        for (int idx : corecte) {
            if (idx >= 0 && static_cast<size_t>(idx) < n)
                corecteFiltrate.push_back(idx);
        }
        if (corecteFiltrate.empty()) {
            std::cout << "[Hint] Exista mai multe raspunsuri corecte.\n";
            return;
        }
        std::uniform_int_distribution<size_t> dist(0, corecteFiltrate.size() - 1);
        int dezvaluie = corecteFiltrate[dist(rng)];
        std::cout << "[Hint] Sunt exact " << corecteFiltrate.size() << " variante corecte. Una dintre ele este: "
                  << (dezvaluie + 1) << ") " << variante[static_cast<size_t>(dezvaluie)] << "\n";

        // optional, elimina si o varianta gresita daca exista
        std::vector<int> gresite;
        gresite.reserve(n);
        {
            std::vector<bool> eCorecta(n, false);
            for (int idx : corecteFiltrate) if (idx >= 0 && static_cast<size_t>(idx) < n) eCorecta[static_cast<size_t>(idx)] = true;
            for (size_t i = 0; i < n; ++i) if (!eCorecta[i]) gresite.push_back(static_cast<int>(i));
        }
        if (!gresite.empty()) {
            std::uniform_int_distribution<size_t> distG(0, gresite.size() - 1);
            int elimina = gresite[distG(rng)];
            std::cout << "[Hint] Poti elimina: " << (elimina + 1) << ") " << variante[static_cast<size_t>(elimina)] << "\n";
        }
        return;
    }

    // pt simpla si adevarat/fals: afla indicele corect prin probare cu verificaRaspuns
    int indiceCorect = -1;
    for (size_t i = 0; i < n; ++i) {
        if (intrebare->verificaRaspuns(std::vector<int>{static_cast<int>(i)})) {
            indiceCorect = static_cast<int>(i);
            break;
        }
    }

    if (n == 2) {
        // Adevarat/Fals: spune care nu este corect sau care este corect [mai trebuie gandit :) ]
        if (indiceCorect >= 0) {
            int gresit = 1 - indiceCorect; // 0 sau 1
            std::cout << "[Hint] Varianta gresita este: " << (gresit + 1) << ") "
                      << variante[static_cast<size_t>(gresit)] << "\n";
        } else {
            std::cout << "[Hint] Alege intre cele doua optiuni; una singura este corecta.\n";
        }
        return;
    }

    // intrebare simpla (single-choice cu >= 3 variante)
    // elimina o varianta gresita aleator si optional indica cate raman
    std::vector<int> gresite;
    gresite.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        if (static_cast<int>(i) != indiceCorect)
            gresite.push_back(static_cast<int>(i));
    }
    if (!gresite.empty()) {
        std::uniform_int_distribution<size_t> dist(0, gresite.size() - 1);
        int elimina = gresite[dist(rng)];
        std::cout << "[Hint] Eliminam o varianta gresita: " << (elimina + 1) << ") "
                  << variante[static_cast<size_t>(elimina)] << "\n";
        std::cout << "[Hint] Raspunsul corect se afla printre cele ramase." << "\n";
    } else {
        std::cout << "[Hint] Alege o singura varianta." << "\n";
    }
}



#endif //OOP_UTIL_H