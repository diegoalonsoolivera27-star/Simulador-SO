#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

const int ORDEN_MAX = 10;          
vector<int> libres[ORDEN_MAX + 1]; 

int orden(int kb) {                
    int k = 0;
    while ((1 << k) < kb) k++;
    return k;
}

int asignar(int kb) {
    int k = orden(kb), j = k;
    while (j <= ORDEN_MAX && libres[j].empty()) j++;   
    if (j > ORDEN_MAX) { cout << "Sin memoria\n"; return -1; }

    int dir = libres[j].back();
    libres[j].pop_back();
    while (j > k) {                                    
        j--;
        libres[j].push_back(dir + (1 << j));
        cout << "  divide: queda libre [" << dir + (1 << j) << "] de " << (1 << j) << " KB\n";
    }
    cout << "Asignado: pidio " << kb << " KB, recibio " << (1 << k) << " KB en dir " << dir << "\n";
    return dir;
}

void liberar(int dir, int kb) {
    int k = orden(kb);
    while (k < ORDEN_MAX) {
        int buddy = dir ^ (1 << k);                    
        auto it = find(libres[k].begin(), libres[k].end(), buddy);
        if (it == libres[k].end()) break;              
        libres[k].erase(it);
        cout << "  fusiona [" << dir << "] y [" << buddy << "] -> " << (2 << k) << " KB\n";
        dir = min(dir, buddy);
        k++;
    }
    libres[k].push_back(dir);
    cout << "Liberado: bloque libre de " << (1 << k) << " KB en dir " << dir << "\n";
}

int main() {
    libres[ORDEN_MAX].push_back(0);    

    int a = asignar(100);              
    int b = asignar(200);              

    cout << "\n";
    liberar(a, 100);
    liberar(b, 200);                   
}