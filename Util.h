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

inline void afiseazaHint(const Intrebare& intrebare, std::mt19937& rng) {
    const auto& variante = intrebare.getVariante();
    const auto corecte = intrebare.getRaspunsuriCorecte();

    if (intrebare.getTip() == TipIntrebare::Multipla) {
        std::cout << "Sugestie: Exista " << corecte.size()
                  << " raspunsuri corecte pentru aceasta intrebare.\n";
        return;
    }

    std::uniform_int_distribution<size_t> dist(0, corecte.size() - 1);
    const auto indexSelectat = corecte[dist(rng)];
    if (static_cast<size_t>(indexSelectat) < variante.size()) {
        std::cout << "Sugestie: Gandeste-te la varianta \"" << variante[indexSelectat] << "\".\n";
    } else {
        std::cout << "Sugestie: Raspunsul corect se afla printre optiunile numerotate mici.\n";
    }
}


#endif //OOP_UTIL_H