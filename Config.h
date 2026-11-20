#pragma once
#include <iostream>
#include <limits>
using namespace std;

const int SPLIT_MIN = 32; // KB, tamaño mínimo de split

struct Proceso{
    int id;
    int tamano; // KB
    int cuanto; // tiempo dde CPU que necesita
    int llegada; // reloj de llegada
    int primerTurno = -1;
    int fin = -1;
};

struct config{
    int memoriaTotal; // (1024, 4096 y 8192)
    int tamMaxPro;
    int cuantoMaxPro; // tiempo máximo de CPU que puede tener
    int cuantoSistema;
    int velocidadMS; // pausa entre cada tick del reloj
};

inline int pedirEntero(int min, int max, bool mostrarRango){
    if (mostrarRango){
        cout << "[" << min << " - " << max << "] : ";
    }
    int v;
    while (true){
        if (cin >> v && v >= min && v<= max){
            return v;
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Valor invalido, intente de nuevo: ";
        }
    }
}

inline config leerConfig(){
    config c;
    int mb;
    do{
        cout << "Memoria en MB (1, 4, 8): ";
        mb = pedirEntero(1, 8, false);
    } while (mb != 1 && mb != 4 && mb != 8);
    c.memoriaTotal = mb * 1024; // convertir a KB

    cout << "Tamaño máximo de procesos (KB) ";
    c.tamMaxPro = pedirEntero(1, c.memoriaTotal, true);

    cout << "Cuanto máximo por proceso ";
    c.cuantoMaxPro = pedirEntero(1, 100, true);

    cout << "Cuanto del sistema ";
    c.cuantoSistema = pedirEntero(1, c.cuantoMaxPro, true);

    cout << "Velocidad del sistema (ms) ";
    c.velocidadMS = pedirEntero(1, 5000, true);

    return c;
}