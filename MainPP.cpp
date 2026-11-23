#include "Config.h"
#include "Memoria.h"
#include "PeorAjuste.h"

#include <iostream>
#include <limits>
#include <map>
#include <string>

using namespace std;

int leerEntero(const string& mensaje, int minimo, int maximo) {
    int valor;
    while (true) {
        cout << mensaje << " [" << minimo << "-" << maximo << "]: ";
        if (cin >> valor && valor >= minimo && valor <= maximo) return valor;
        cout << "Entrada invalida. Escribe un numero dentro del rango.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void mostrarEstado(const PeorAjuste& memoria, const map<int, int>& restantes) {
    cout << "\nMemoria: " << memoria.estado() << '\n';
    cout << "Libre total: " << memoria.libreTotal() << " KB\n";
    if (restantes.empty()) {
        cout << "Procesos activos: ninguno\n";
    } else {
        cout << "Cuanto restante por proceso:\n";
        for (const auto& [id, cuanto] : restantes) {
            cout << "  Proceso " << id << ": " << cuanto << '\n';
        }
    }
    cout << '\n';
}

int main() {
    cout << "Demostracion interactiva: Peor Ajuste\n";
    cout << "Elige la memoria total.\n";
    int memoriaMB;
    while (true) {
        memoriaMB = leerEntero("Memoria en MB (1, 4 u 8)", 1, 8);
        if (memoriaMB == 1 || memoriaMB == 4 || memoriaMB == 8) break;
        cout << "Solo se permite 1, 4 u 8 MB.\n";
    }

    const int memoriaKB = memoriaMB * 1024;
    PeorAjuste memoria(memoriaKB);
    map<int, int> cuantoRestante;
    int siguienteId = 1;

    cout << "\nMemoria inicial: " << memoria.estado() << "\n";
    bool continuar = true;
    while (continuar) {
        cout << "\n1) Ingresar proceso\n"
                  << "2) Dar turno de CPU a un proceso\n"
                  << "3) Mostrar estado\n"
                  << "0) Salir\n";
        int opcion = leerEntero("Opcion", 0, 3);

        if (opcion == 1) {
            int tamano = leerEntero("Tamano del proceso en KB", 1, memoriaKB);
            int cuanto = leerEntero("Cuanto de CPU que necesita", 1, 100);
            Proceso nuevo{};
            nuevo.id = siguienteId++;
            nuevo.tamano = tamano;
            nuevo.cuanto = cuanto;

            cout << "\nLlega [" << nuevo.id << ',' << nuevo.tamano << ','
                      << nuevo.cuanto << "]\n";
            if (memoria.asignar(nuevo)) {
                cuantoRestante[nuevo.id] = nuevo.cuanto;
                cout << "Asignado con Peor Ajuste.\n";
            } else {
                cout << "No se asigno: no hay un hueco contiguo suficientemente grande.\n";
            }
            mostrarEstado(memoria, cuantoRestante);
        } else if (opcion == 2) {
            if (cuantoRestante.empty()) {
                cout << "No hay procesos activos. Ingresa uno primero.\n";
                continue;
            }
            int id = leerEntero("ID del proceso que recibira CPU", 1, siguienteId - 1);
            auto proceso = cuantoRestante.find(id);
            if (proceso == cuantoRestante.end()) {
                cout << "Ese proceso no esta activo.\n";
                continue;
            }
            int usado = leerEntero("Unidades de CPU consumidas en este turno",
                                   1, proceso->second);
            proceso->second -= usado;
            if (proceso->second == 0) {
                memoria.liberar(id);
                cuantoRestante.erase(proceso);
                cout << "El proceso termino y su memoria fue liberada.\n";
            } else {
                memoria.actualizarCuanto(id, proceso->second);
                cout << "El proceso sigue activo; se actualizo su cuanto.\n";
            }
            mostrarEstado(memoria, cuantoRestante);
        } else if (opcion == 3) {
            mostrarEstado(memoria, cuantoRestante);
        } else {
            continuar = false;
        }
    }

    cout << "Demostracion terminada.\n";
    return 0;
}