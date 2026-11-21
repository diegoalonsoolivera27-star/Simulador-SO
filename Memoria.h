#pragma once
#include "Config.h"

class Memoria {
public:
    virtual ~Memoria() {} // destructor virtual, necesario en clases con herencia
    virtual bool asignar(const Proceso& p) = 0; // mete el proceso en memoria
    virtual void liberar(int id) = 0; // libera la memoria del proceso con ese id
    virtual void actualizarCuanto(int id, int cuanto) = 0; // refresca el cuanto que se muestra del proceso
    virtual string estado() const = 0; // texto de la memoria en orden de direcciones
    virtual int libreTotal() const = 0;
};