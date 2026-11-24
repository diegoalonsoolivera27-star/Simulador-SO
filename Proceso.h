#pragma once

struct Proceso {
    int id;
    int tamano;       // KB
    int cuanto;       // tiempo de CPU que le falta
    int llegada;      // reloj al generarse
    int primerTurno = -1;
    int fin = -1;
};