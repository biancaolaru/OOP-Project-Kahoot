//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_QUIZ_H
#define OOP_QUIZ_H

#include <vector>
#include <iostream>
#include "Intrebare.h"

class Quiz {

    std::vector<Intrebare> intrebari;

public:
    explicit Quiz(const std::vector<Intrebare>& intrebari = {});
    Quiz(const Quiz& other);
    Quiz& operator=(const Quiz& other);
    ~Quiz() = default;

    const std::vector<Intrebare>& getIntrebari() const;

    void amestecaIntrebari();

    friend std::ostream& operator<<(std::ostream& out, const Quiz& c);
};

#endif //OOP_QUIZ_H