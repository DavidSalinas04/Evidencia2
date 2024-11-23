#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <climits>
#include <sstream>

using namespace std;

// Estructura para representar una arista
struct Arista {
    int u, v;
    int peso;
};

// Función para encontrar el padre de un nodo (para Union-Find)
int encontrarPadre(int nodo, vector<int>& padre) {
    if (padre[nodo] == nodo)
        return nodo;
    return padre[nodo] = encontrarPadre(padre[nodo], padre);
}

// Función para unir dos conjuntos (para Union-Find)
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

// Función para calcular el MST usando el algoritmo de Kruskal
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

// Función para resolver el TSP usando el vecino más cercano
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

// Función de BFS para el algoritmo de Edmonds-Karp
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

// Función para calcular el flujo máximo usando el algoritmo de Ford-Fulkerson (Edmonds-Karp)
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

    vector<pair<int, int>> coordenadas(n);
    for (int i = 0; i < n; i++) {
        char paren1, comma, paren2;
        archivo >> paren1 >> coordenadas[i].first >> comma >> coordenadas[i].second >> paren2;
    }

    archivo.close();

    // Punto 1: Calcular el MST y mostrarlo
    vector<Arista> mst = kruskalMST(n, matrizAdy);

    cout << "Forma de cablear las colonias con fibra:" << endl;
    for (auto& arista : mst) {
        cout << "(" << char('A' + arista.u) << ", " << char('A' + arista.v) << ")" << endl;
    }

    // Punto 2: Resolver el TSP y mostrar la ruta más corta
    auto resultadoTSP = tspNearestNeighbor(n, matrizAdy);

    cout << "\nRuta mas corta:" << endl;
    for (int nodo : resultadoTSP.first) {
        cout << char('A' + nodo) << " -> ";
    }
    cout << "FIN" << endl;

    cout << "Costo total de la ruta: " << resultadoTSP.second << endl;

    // Punto 3: Calcular el flujo máximo y mostrar el resultado
    int fuente, destino;
    cout << "\nIntroduce el nodo fuente (0-" << n - 1 << "): ";
    cin >> fuente;
    cout << "Introduce el nodo destino (0-" << n - 1 << "): ";
    cin >> destino;

    int flujoMaximo = fordFulkerson(capacidad, fuente, destino);
    cout << "El flujo máximo desde " << char('A' + fuente) << " hasta " << char('A' + destino) << " es: " << flujoMaximo << endl;

    // Punto 4: Mostrar la lista de polígonos
    cout << "\nLista de polígonos (nodos en formato (x,y)):" << endl;
    for (const auto& coord : coordenadas) {
        cout << "(" << coord.first << ", " << coord.second << ")" << endl;
    }

    return 0;
}
