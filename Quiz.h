//
// Created by Bianca on 11/17/2025.
//

#ifndef OOP_QUIZ_H
#define OOP_QUIZ_H

#include <vector>
#include <memory>
#include <algorithm>
#include "Intrebare.h"

class Quiz {
private:
    std::vector<std::unique_ptr<Intrebare>> intrebari;

public:
    // swap friend pt copy-and-swap
    friend void swap(Quiz& a, Quiz& b) noexcept { a.intrebari.swap(b.intrebari); }

    Quiz() = default;

    // constructor care primeste vector de unique_ptr
    Quiz(std::vector<std::unique_ptr<Intrebare>>&& intrebariVec) {
        for (auto& i : intrebariVec)
            intrebari.push_back(std::move(i));
    }

    // cnstructor copiere
    Quiz(const Quiz& other) {
        intrebari.reserve(other.intrebari.size());
        for (const auto& q : other.intrebari) {
            if (q) intrebari.push_back(q->clone());
        }
    }

    Quiz& operator=(Quiz other) {
        swap(*this, other);
        return *this;
    }

    Quiz(Quiz&&) noexcept = default;
    Quiz& operator=(Quiz&&) noexcept = default;

    // fct de nivel inalt
    size_t size() const noexcept;
    Quiz takeFirstNCloned(size_t n) const;

    std::vector<std::unique_ptr<Intrebare>>& getIntrebari() {
        return intrebari;
    }

    const std::vector<std::unique_ptr<Intrebare>>& getIntrebari() const {
        return intrebari;
    }

    void amestecaIntrebari();

    // supraincarcare operator<< pentru afisare
    friend std::ostream& operator<<(std::ostream& out, const Quiz& q) {
        for (const auto& i : q.intrebari)
            i->afiseaza();
        return out;
    }
};

#endif //OOP_QUIZ_H
