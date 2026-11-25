#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include "Memoria.h"

struct Bloque{
    int dir; // dirección inicial en kb
    int tam; // tamaño del bloque(potencia de 2)
    Proceso p; // proceso que lo ocupa
};

class Buddy : public Memoria{
    private:
        int ordenMax;
        vector<vector<int>> libres; // libres[k] son las direcciones de bloques libres de 2 elevado a k KB
        map<int, Bloque> ocupados; // id del proceso -> su bloque

        int orden(int kb) const{ // devuelve el k de la potencia de 2 más chica que cabe
            int k = 0;
            while ((1 << k) < kb) // mientras ese tamaño no alcance para kb, sube de orden
                k++; // k donde 2 elevado a la k >= kb
            return k;
        }
    public:
        Buddy(int memoriaTotal){
            ordenMax = orden(memoriaTotal); // calcula el orden de la memoria total
            libres.resize(ordenMax + 1); // crea 13 vectores (0 a 12) para cada tamaño posible
            libres[ordenMax].push_back(0); // agrega la diección 0 al inicio que significa 1 solo bloque que ocupa toda la memoria
        }

        int libreTotal() const override{
            int total = 0;
            for (int i = 0; i < ordenMax; i++){
                total += libres[i].size() * (1 << i); // cuantos KB libres hay en total
            }
            return total;
        }

        bool asignar(const Proceso &p) override{
            int k = orden(max(p.tamano, SPLIT_MIN)); // orden de bloque que necesita 
            int j = k;
            while (j <= ordenMax && libres[j].empty()) // busca el bloque libre más chico
                j++;
            if (j > ordenMax)
                return false; // no hay bloque libre
            int dir = libres[j].back(); // toma un bloque libre de orden j
            libres[j].pop_back();
            while(j > k){ // lo divide hasta llegar a orden k
                j--;
                libres[j].push_back(dir + (1 << j)); // la mitad derecha queda libre
            }
            ocupados[p.id] = {dir, 1 << k, p}; // la mitad izq del proceso
            return true;
        }

        void liberar(int id) override {
            auto it = ocupados.find(id); // busca el bloque del proceso
            if (it == ocupados.end()) // si no existe, no hace nada
                return;

            int dir = it->second.dir;
            int k = orden(it->second.tam);
            ocupados.erase(it); // el proceso ya no ocupa un bloque

            while(k < ordenMax){
                int buddy = dir ^(1 << k); // dirección del buddy
                auto b = find(libres[k].begin(), libres[k].end(), buddy); // busca buddy en libres desde inicio a fin
                if (b == libres[k].end())
                    break; // el buddy no está libre y no se puede fusionar
                libres[k].erase(b); // se quita buddy de la lista de libres
                dir = min(dir, buddy); // el bloque nuevo empoeza en la dir menor
                k++; // el bloque es del doble de tamaño
            }
            libres[k].push_back(dir); //el nuevo bloque queda libre
        }

        void actualizarCuanto(int id, int cuanto) override{
            auto it = ocupados.find(id);
            if (it != ocupados.end())
                it->second.p.cuanto = cuanto;
        }

        string estado() const override{
            map<int, string> bloques; // dirección / texto - pero se ordenan por dirección

            for (const auto& par : ocupados) {
                const Bloque& b = par.second;
                bloques[b.dir] = pts(b.p);
                int frag = b.tam - b.p.tamano;
                if (frag > 0)
                    bloques[b.dir + b.p.tamano] = "[h," + to_string(frag) + ",0]";
            }
            
            for (int i=0; i<ordenMax; i++){ // bloques libres: [0, tamaño del bloque, 0]
                for (int dir : libres[i]){
                    bloques[dir] = "[0," + to_string(1 << i) + ",0]";   
                }
            }
            
            string s;
            for (const auto &par : bloques){
                s += par.second + " ";            
            }
            return s;    
        }
};