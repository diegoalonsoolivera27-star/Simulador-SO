#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <algorithm>
#include <thread>
#include <chrono>
#include "Config.h"

const int MAX_COLA = 5; // TEMPORAL
int contadorId = 0;
int reloj = 0;

Proceso generarProceso(const config& c){
    Proceso p;
    p.id = ++contadorId;
    p.tamano = rand() % c.tamMaxPro + 1;
    p.cuanto = rand() % c.cuantoMaxPro + 1;
    p.llegada = reloj;
    return p;
}

string pts(const Proceso& p){
    return "[" + to_string(p.id) + ", " + to_string(p.tamano) + ", " + to_string(p.cuanto) + "]";
}

void imprimirCola(const deque<Proceso>& cola){
    cout << "\n\n" << "Cola de Listos: " << endl;
    for(const Proceso& p : cola)
        cout << pts(p) << "\n";
}

int main(){
    srand(time(NULL));
    config c = leerConfig();
    deque<Proceso> colaListos;

    for(int ciclo = 1; ciclo <= 20; ciclo++){
        while((int)colaListos.size() < MAX_COLA)
            colaListos.push_back(generarProceso(c));
        
        cout << "Ciclo " << ciclo << endl;
        Proceso p = colaListos.front();
        colaListos.pop_front();
        if(p.primerTurno == -1) 
            p.primerTurno = reloj;
        cout << "Atendiendo proceso " << pts(p) << endl;

        int usado = min(c.cuantoSistema, p.cuanto);
        p.cuanto -= usado;
        reloj += usado;

        if (p.cuanto == 0){
            p.fin = reloj;
            cout << "Proceso " << p.id << " terminó" << endl;
        } else{
            colaListos.push_back(p);
        }
        
        cout << "Fin de ciclo (t= " << reloj << ")";
        imprimirCola(colaListos);
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(c.velocidadMS));
    }
}

