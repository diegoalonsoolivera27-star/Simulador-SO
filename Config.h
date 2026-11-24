#pragma once
#include <iostream>
#include <limits>
#include <string>
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

struct Estadisticas {
    int atendidos = 0;       // cuantos procesos terminaron
    long sumaRetorno = 0;    // tiempo total de cada proceso en el sistema
    long sumaRespuesta = 0;  // espera hasta su primer turno
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
    int mb, opc = 0;
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

    cout << "Velocidad del sistema (1 - rápido / 2 - normal / 3 - lento): ";
    do {
        cin >> opc;
        switch (opc){
        case 1: c.velocidadMS = 1000;
            break;
        case 2: c.velocidadMS = 2000;
            break;
        case 3: c.velocidadMS = 3000;
            break;
        default:
            cout << "Opción inválida, intente de nuevo: ";
            break;
        }
    } while (opc != 1 && opc != 2 && opc != 3);
    return c;
}

string pts(const Proceso& p){ //convierte el proceso a la cadena de tecto "proceso to string"
    return "[" + to_string(p.id) + "," + to_string(p.tamano) + "," + to_string(p.cuanto) + "]";
}