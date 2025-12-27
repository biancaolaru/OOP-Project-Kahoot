//
// ierarhie de exceptii
//
#pragma once

#include <stdexcept>
#include <string>

// baza
class EroareAplicatie : public std::runtime_error {
public:
    explicit EroareAplicatie(const std::string& mesaj)
        : std::runtime_error(mesaj) {}
};

// eroare fisier
class EroareFisier : public EroareAplicatie {
public:
    explicit EroareFisier(const std::string& mesaj)
        : EroareAplicatie(mesaj) {}
};

// eroare date
class EroareFormat : public EroareAplicatie {
public:
    explicit EroareFormat(const std::string& mesaj)
        : EroareAplicatie(mesaj) {}
};

// eroare de logica a jocului
class EroareStareJoc : public EroareAplicatie {
public:
    explicit EroareStareJoc(const std::string& mesaj)
        : EroareAplicatie(mesaj) {}
};
