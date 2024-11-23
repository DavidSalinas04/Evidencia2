#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
#include <climits>
#include <cfloat>
#include <string>
#include <thread>

using namespace std;

// Punto 1
struct Arista {
    int u, v;
    int peso;
};

int encontrarPadre(int nodo, vector<int>& padre) {
    if (padre[nodo] == nodo)
        return nodo;
    return padre[nodo] = encontrarPadre(padre[nodo], padre);
}

void unirConjuntos(int u, int v, vector<int>& padre, vector<int>& rango) {
    int raizU = encontrarPadre(u, padre);
    int raizV = encontrarPadre(v, padre);
    if (raizU != raizV) {
        if (rango[raizU] < rango[raizV])
            padre[raizU] = raizV;
        else if (rango[raizU] > rango[raizV])
            padre[raizV] = raizU;
        else {
            padre[raizV] = raizU;
            rango[raizU]++;
        }
    }
}

vector<Arista> kruskalMST(int n, vector<vector<int>>& matrizAdy) {
    vector<Arista> aristas;
    vector<Arista> mst;
    vector<int> padre(n), rango(n, 0);

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (matrizAdy[i][j] != 0) {
                aristas.push_back({i, j, matrizAdy[i][j]});
            }
        }
    }

    sort(aristas.begin(), aristas.end(), [](Arista a, Arista b) {
        return a.peso < b.peso;
    });

    for (int i = 0; i < n; i++)
        padre[i] = i;

    for (auto& arista : aristas) {
        if (encontrarPadre(arista.u, padre) != encontrarPadre(arista.v, padre)) {
            mst.push_back(arista);
            unirConjuntos(arista.u, arista.v, padre, rango);
        }
    }

    return mst;
}

// Punto 2
pair<vector<int>, int> tspNearestNeighbor(int n, const vector<vector<int>>& matrizAdy) {
    vector<bool> visitado(n, false);
    vector<int> ruta;
    int costoTotal = 0;

    int nodoActual = 0;
    ruta.push_back(nodoActual);
    visitado[nodoActual] = true;

    for (int i = 1; i < n; i++) {
        int nodoMasCercano = -1;
        int distanciaMinima = INT_MAX;

        for (int j = 0; j < n; j++) {
            if (!visitado[j] && matrizAdy[nodoActual][j] < distanciaMinima) {
                nodoMasCercano = j;
                distanciaMinima = matrizAdy[nodoActual][j];
            }
        }

        costoTotal += distanciaMinima;
        nodoActual = nodoMasCercano;
        ruta.push_back(nodoActual);
        visitado[nodoActual] = true;
    }

    costoTotal += matrizAdy[nodoActual][0];
    ruta.push_back(0);

    return {ruta, costoTotal};
}

// Punto 3: Flujo máximo usando Edmonds-Karp
bool bfs(const vector<vector<int>>& capacidad, vector<vector<int>>& residual, vector<int>& padre, int fuente, int destino) {
    int n = capacidad.size();
    vector<bool> visitado(n, false);
    queue<int> q;

    q.push(fuente);
    visitado[fuente] = true;
    padre[fuente] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < n; v++) {
            if (!visitado[v] && residual[u][v] > 0) {
                q.push(v);
                padre[v] = u;
                visitado[v] = true;

                if (v == destino)
                    return true;
            }
        }
    }
    return false;
}

int fordFulkerson(const vector<vector<int>>& capacidad, int fuente, int destino) {
    int n = capacidad.size();
    vector<vector<int>> residual = capacidad;
    vector<int> padre(n);
    int flujoMaximo = 0;

    while (bfs(capacidad, residual, padre, fuente, destino)) {
        int flujoCamino = INT_MAX;
        for (int v = destino; v != fuente; v = padre[v]) {
            int u = padre[v];
            flujoCamino = min(flujoCamino, residual[u][v]);
        }

        for (int v = destino; v != fuente; v = padre[v]) {
            int u = padre[v];
            residual[u][v] -= flujoCamino;
            residual[v][u] += flujoCamino;
        }

        flujoMaximo += flujoCamino;
    }

    return flujoMaximo;
}

// Punto 4: Encontrar la central más cercana
pair<int, double> encontrarCentralMasCercana(int x, int y, const vector<pair<int, int>>& centrales) {
    int indiceMasCercano = -1;
    double distanciaMinima = DBL_MAX;

    for (int i = 0; i < centrales.size(); i++) {
        int cx = centrales[i].first;
        int cy = centrales[i].second;
        double distancia = sqrt(pow(x - cx, 2) + pow(y - cy, 2));
        if (distancia < distanciaMinima) {
            distanciaMinima = distancia;
            indiceMasCercano = i;
        }
    }

    return {indiceMasCercano, distanciaMinima};
}

void menu(int n, vector<vector<int>> matrizAdy, vector<vector<int>> capacidad, vector<pair<int, int>> centrales) {
    int opcion;
    cout<<endl; 
    cout<<"-------------------"<<endl;
    cout<<"Evidencia 2 - Algoritmos y Estructuras de Datos"<<endl;
    cout<<"-------------------"<<endl;
    cout<<"Jose Ignacio Paez"<<endl; 
    cout<<"David Salinas"<<endl; 
    cout<<"-------------------"<<endl;
    this_thread::sleep_for(std::chrono::seconds(3));
    do{
        auto resultadoTSP = tspNearestNeighbor(n, matrizAdy);
        auto mst = kruskalMST(n, matrizAdy);

        cout<<"Menu"<<endl;
        cout << "1. Forma de cablear las colonias con fibra" << endl;
        cout << "2. Ruta mas corta" << endl;
        cout << "3. Flujo máximo" << endl;
        cout << "4. Central más cercana" << endl;
        cout << "5. Salir" << endl;
        cin >> opcion;
        
        switch (opcion) {
            case 1: {
                cout << "Forma de cablear las colonias con fibra" << endl;
                for (auto& arista : mst) {
                    cout << "(" << char('A' + arista.u) << ", " << char('A' + arista.v) << ")" << endl;
                }
                cout<<"-------------------"<<endl;
                this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }
            case 2: {
                cout << "Ruta mas corta:" << endl;
                for (int nodo : resultadoTSP.first) {
                    cout << char('A' + nodo) << " -> ";
                }
                cout << "FIN" << endl;
                cout << "Costo total de la ruta: " << resultadoTSP.second << endl;
                cout<<"-------------------"<<endl;
                this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }
            case 3: {
                int fuente, destino;
                cout << "Introduce el nodo fuente (0-" << n - 1 << "): ";
                cin >> fuente;
                cout << "Introduce el nodo destino (0-" << n - 1 << "): ";
                cin >> destino;
                int flujoMaximo = fordFulkerson(capacidad, fuente, destino);
                cout << "El flujo máximo desde " << char('A' + fuente) << " hasta " << char('A' + destino) << " es: " << flujoMaximo << endl;
                cout<<"-------------------"<<endl;
                this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }
            case 4: {
                int x, y;
                cout << "Introduce las coordenadas de la nueva casa (x y): ";
                cin >> x >> y;
                auto resultadoCentral = encontrarCentralMasCercana(x, y, centrales);
                cout << "La central mas cercana es la " << resultadoCentral.first << " a una distancia de " << resultadoCentral.second << "km"<<endl;
                cout<<"-------------------"<<endl;
                this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }
            case 5: {
                cout<<"Saliendo..."<<endl;
                this_thread::sleep_for(std::chrono::seconds(2));
                
                break;
            }
            default: {
                cerr << "Opción no válida" << endl;
                cout<<"-------------------"<<endl;
                this_thread::sleep_for(std::chrono::seconds(2));
                break;
            }
            }
    }while(opcion!=5); 
}

    

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <nombre_archivo>" << endl;
        return 1;
    }

    string nombreArchivo = argv[1];
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombreArchivo << endl;
        return 1;
    }

    int n;
    archivo >> n;

    vector<vector<int>> matrizAdy(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            archivo >> matrizAdy[i][j];
        }
    }

    vector<vector<int>> capacidad(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            archivo >> capacidad[i][j];
        }
    }

    vector<pair<int, int>> centrales(n);
    for (int i = 0; i < n; i++) {
        string coordenada;
        archivo >> coordenada;

        size_t coma = coordenada.find(',');
        int x = stoi(coordenada.substr(1, coma - 1));
        int y = stoi(coordenada.substr(coma + 1, coordenada.size() - coma - 2));

        centrales[i] = {x, y};
    }

    archivo.close();

    
    menu(n, matrizAdy, capacidad, centrales);
    

}
