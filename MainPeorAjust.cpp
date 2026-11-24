#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <algorithm>
#include <thread> // thread y this_thread, hilos y pausas
#include <chrono>// duraciones de tiempo (milliseconds, seconds)
#include <atomic> // para compartir variables entre hilos
#include "Config.h"
#include "Memoria.h"
#include "PeorAjuste.h"

const int MAX_COLA = 5;
atomic <bool> parar(false);   // orden de terminar; la activa el hilo del teclado
atomic <int> velocidad(0);    // pausa entre ciclos en ms; el teclado la cambia
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

void imprimirCola(const deque<Proceso>& cola){
    cout << "Lista RR: " << endl;
    for(const Proceso& p : cola)
        cout << pts(p);
}

// Para leer el teclado sin detener la simulacion
void leerTeclas(){
    char t;
    while (!parar && cin >> t){ // Espera a la tecla con Enter
        if (t == 's'){
            parar = true;
        }
        else if (t == '+') velocidad = max(1, velocidad / 2); // pausa a la mitad, minimo 1 ms
        else if (t == '-') velocidad = min(5000, velocidad * 2);  // pausa al doble, maximo 5000 ms
    }
}

int menu(){
    int opcion;        
    cout<<"Elije el tipo de memoria que desea usar (1. Peor Ajuste / 2. Buddy System): ";
    do {
        cin>>opcion;
        if (opcion != 1 && opcion != 2)
            cout<<"Opción inválida, intenta de nuevo."<<endl;
    } while (opcion != 1 && opcion != 2);
    return opcion;
}

int main(){
    srand(time(NULL));
    config c = leerConfig();
    int opcion = menu(); // el menu regresa un numero que indica la opcion de memoria que se desea usar
    Memoria* memoria = nullptr;
    if (opcion == 1) {
        memoria = new PeorAjuste(c.memoriaTotal);
    } else if(opcion == 2){
        cout << "buddy system"<<endl;
    } else {
        cout<<"Opcion no valida";
        return 0;
    }
    deque<Proceso> colaListos;
    Estadisticas est;
    velocidad = c.velocidadMS;
    int ciclo = 0;
    cout << "\ns : Terminar ejecución | + : Más rapido | - : Más lento\n" << endl;
    this_thread::sleep_for(chrono::seconds(1)); // pausa de 1 segundo para leer instrucciones
    thread teclado(leerTeclas);  // crea y arranca el hilo del teclado; corre en paralelo para no detener la ejecución del simulador

    while(!parar){
        ciclo++;
        cout << "Ciclo " << ciclo << endl;
        if((int)colaListos.size() < MAX_COLA){// mientras la cola no tenga más procesos que MAX_COLA
            Proceso nuevo = generarProceso(c); // genera un proceso aleatorio
            cout << "Proceso nuevo: " << pts(nuevo) <<endl;
            if(memoria->asignar(nuevo)){
                colaListos.push_back(nuevo); // añade a la cola un proceso aleatorio generado
            }else{
                cout<<"No hay memoria suficiente para el proceso "<< pts(nuevo) << endl;
            }  
        }

        if(colaListos.empty()){ // Si no hay nada que atender, espera hasta generar un proceso en sig. ciclo
            this_thread::sleep_for(chrono::milliseconds(velocidad));
            continue;
        
        }
        
        cout << memoria->estado() << "\n" << endl;
        Proceso p = colaListos.front();
        colaListos.pop_front();
        if(p.primerTurno == -1) // si nunca se ha atendido
            p.primerTurno = reloj; // guarda cuando recibe el CPU por primera vez
        cout << "Atendiendo proceso: " << pts(p) << endl;

        int usado = min(c.cuantoSistema, p.cuanto); // corre un cuanto completo, o menos si ya no requiere más
        p.cuanto -= usado; // remueve el tiempo usado
        reloj += usado; // lo aumenta en la estadística
        memoria->actualizarCuanto(p.id, p.cuanto); // actualiza el cuanto en la memoria

        if (p.cuanto == 0){ // si terminó, se imprime el mensaje
            p.fin = reloj;
            memoria->liberar(p.id);//liberar memoria del proceso
            cout << "Proceso " << p.id << " terminado \n"  << endl;
            est.atendidos++;
            est.sumaRespuesta += (p.primerTurno - p.llegada);
            est.sumaRetorno += (p.fin - p.llegada);
        } else{ // si no, se manda a la cola de listos
            colaListos.push_back(p);
        }
        
        imprimirCola(colaListos);
        cout << "\nEstado de la memoria:" << endl;
        cout << memoria->estado() << endl;
        cout << "Memoria libre: " << memoria->libreTotal() << " KB\n" << endl;
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(velocidad)); // usa el valor actual que puede modificarse
    }
    teclado.join();// espera a que el hilo del teclado termine antes de cerrar el programa
    cout << "\nSimulación terminada" << endl;
    /*cout << "\nEstadísticas" << endl;
    cout << "Procesos atendidos: " << est.atendidos << endl;
    if(est.atendidos > 0){
        cout << "Tiempo medio de atencion: " << (double)est.sumaRetorno / est.atendidos << endl;
        cout << "Tiempo medio de respuesta: " << (double)est.sumaRespuesta / est.atendidos << endl;
    }
    */
    delete memoria; // libera la memoria asignada al objeto de memoria
    return 0;
}