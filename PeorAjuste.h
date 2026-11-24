#pragma once
#include "Memoria.h"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

// Administrador de memoria contigua con estrategia Peor Ajuste (Worst Fit).
// Las direcciones y tamanos se expresan en KB.
class PeorAjuste final : public Memoria {
private:
    struct Bloque {
        int inicio;
        int tamano;
        int idProceso; // 0 significa bloque libre
        int cuanto;

        bool libre() const { return idProceso == 0; }
    };

    int total;
    vector<Bloque> bloques;

    void unirLibresAdyacentes() {
        for (size_t i = 0; i + 1 < bloques.size();) {
            if (bloques[i].libre() && bloques[i + 1].libre()) {
                bloques[i].tamano += bloques[i + 1].tamano;
                bloques.erase(bloques.begin() + static_cast<ptrdiff_t>(i + 1));
            } else {
                ++i;
            }
        }
    }

public:
    explicit PeorAjuste(int memoriaTotalKB) : total(memoriaTotalKB) {
        if (total <= 0) {
            throw invalid_argument("La memoria total debe ser mayor que cero.");
        }
        bloques.push_back({0, total, 0, 0});
    }

    bool asignar(const Proceso& p) override {
        if (p.id <= 0 || p.tamano <= 0) return false;

        // Evita que una llamada repetida reserve dos veces el mismo proceso.
        for (const Bloque& b : bloques) {
            if (!b.libre() && b.idProceso == p.id) return false;
        }

        // Peor Ajuste: elegir el bloque libre de mayor tamano que pueda alojar p.
        size_t elegido = bloques.size();
        for (size_t i = 0; i < bloques.size(); ++i) {
            if (bloques[i].libre() && bloques[i].tamano >= p.tamano &&
                (elegido == bloques.size() || bloques[i].tamano > bloques[elegido].tamano)) {
                elegido = i;
            }
        }
        if (elegido == bloques.size()) return false;

        Bloque original = bloques[elegido];
        int sobrante = original.tamano - p.tamano;

        // No crear un fragmento libre menor al umbral definido por el equipo.
        // Si no alcanza SPLIT_MIN, el proceso ocupa todo el bloque.
        int asignado = (sobrante >= SPLIT_MIN) ? p.tamano : original.tamano;
        bloques[elegido] = {original.inicio, asignado, p.id, p.cuanto};
        if (asignado < original.tamano) {
            bloques.insert(bloques.begin() + static_cast<ptrdiff_t>(elegido + 1),
                           {original.inicio + asignado, original.tamano - asignado, 0, 0});
        }
        return true;
    }

    void liberar(int id) override {
        for (Bloque& b : bloques) {
            if (!b.libre() && b.idProceso == id) {
                b.idProceso = 0;
                b.cuanto = 0;
                unirLibresAdyacentes();
                return;
            }
        }
    }

    void actualizarCuanto(int id, int cuanto) override {
        for (Bloque& b : bloques) {
            if (!b.libre() && b.idProceso == id) {
                b.cuanto = cuanto;
                return;
            }
        }
    }

    string estado() const override {
        ostringstream salida;
        for (size_t i = 0; i < bloques.size(); ++i) {
            const Bloque& b = bloques[i];
            if (i) salida << ' ';
            // Formato del ejemplo del requerimiento: [Id, tamaño, cuanto].
            // Un hueco se representa con Id=0 y cuanto=0.
            salida << '[' << b.idProceso << ',' << b.tamano << ','
                   << (b.libre() ? 0 : b.cuanto) << ']';
        }
        return salida.str();
    }

    int libreTotal() const override {
        int libre = 0;
        for (const Bloque& b : bloques) {
            if (b.libre()) libre += b.tamano;
        }
        return libre;
    }

    int memoriaTotal() const { return total; }
};
