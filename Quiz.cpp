//
// Created by Bianca on 11/17/2025.
//

#include "Quiz.h"
#include <algorithm>
#include <random>

void Quiz::amestecaIntrebari() {
    std::shuffle(intrebari.begin(), intrebari.end(), std::mt19937{std::random_device{}()});
}
