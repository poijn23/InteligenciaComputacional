/*
 * Actividad 5 - Algoritmo Genetico para optimizacion numerica
 * Inteligencia Artificial (17808) - Universidad Veracruzana
 *
 * Problema:  minimizar f(x) = x1^2 + x2^2,   con x_i en [LIM_INF, LIM_SUP]
 *            optimo conocido: f(0, 0) = 0
 *
 * Representacion binaria:
 *   - Un GEN es un bit (0 o 1).
 *   - Un CROMOSOMA es una lista de genes  -> vector<int>
 *     Los primeros BITS_POR_VARIABLE genes codifican x1 y los siguientes x2.
 *   - La POBLACION es una lista de listas -> vector<vector<int>>
 *     (una lista de cromosomas).
 *
 * Compilar:  g++ -std=c++17 -O2 -o ag main.cpp
 * Ejecutar:  ./ag [semilla_base] [generaciones]
 *            La ejecucion i (1..30) usa la semilla  semilla_base + i - 1.
 */

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace std;

// ---------------------------------------------------------------------------
// 1. REPRESENTACION
// ---------------------------------------------------------------------------
typedef vector<int> Cromosoma;         // lista de genes (cada gen es un bit)
typedef vector<Cromosoma> Poblacion;   // lista de listas (lista de cromosomas)

// ---------------------------------------------------------------------------
// 2. PARAMETROS DEL PROBLEMA
// ---------------------------------------------------------------------------
const int NUM_VARIABLES = 2;
const double LIM_INF = -5.12;
const double LIM_SUP = 5.12;
const int DECIMALES = 6;               // precision pedida para cada variable

// ---------------------------------------------------------------------------
// 3. PARAMETROS DEL ALGORITMO GENETICO
// ---------------------------------------------------------------------------
const int TAM_POBLACION = 100;
const double PROB_CRUZA = 0.9;
const int TAM_TORNEO = 2;              // torneo binario
const int NUM_ELITES = 2;              // mejores individuos que pasan intactos
const int NUM_EJECUCIONES = 30;
int NUM_GENERACIONES = 300;            // se puede cambiar por linea de comandos
unsigned int SEMILLA_BASE = 2026;      // se puede cambiar por linea de comandos

int BITS_POR_VARIABLE;                 // se calcula a partir de la precision
int LONGITUD_CROMOSOMA;                // NUM_VARIABLES * BITS_POR_VARIABLE
double PROB_MUTACION;                  // 1 / LONGITUD_CROMOSOMA

mt19937 generador;                     // generador aleatorio (Mersenne Twister)
long long evaluaciones = 0;            // llamadas a la funcion objetivo


// Numero minimo de bits m tal que 2^m - 1 >= (b - a) * 10^decimales,
// asi cada variable se representa con la precision pedida.
int calcularBitsPorVariable(double a, double b, int decimales) {
    double intervalos = (b - a) * pow(10.0, decimales);
    int m = 1;
    while (pow(2.0, m) - 1 < intervalos) {
        m++;
    }
    return m;
}

double aleatorioReal() {
    return uniform_real_distribution<double>(0.0, 1.0)(generador);
}

int aleatorioEntero(int a, int b) {
    return uniform_int_distribution<int>(a, b)(generador);
}


// ---------------------------------------------------------------------------
// 4. POBLACION INICIAL
// ---------------------------------------------------------------------------
Cromosoma crearCromosoma() {
    Cromosoma cromosoma(LONGITUD_CROMOSOMA);
    for (int i = 0; i < LONGITUD_CROMOSOMA; i++) {
        cromosoma[i] = aleatorioEntero(0, 1);
    }
    return cromosoma;
}

Poblacion inicializarPoblacion() {
    Poblacion poblacion;
    for (int i = 0; i < TAM_POBLACION; i++) {
        poblacion.push_back(crearCromosoma());
    }
    return poblacion;
}


// ---------------------------------------------------------------------------
// 5. DECODIFICACION: genotipo (bits) -> fenotipo (x1, x2)
//    entero = valor del segmento de bits (bit mas significativo primero)
//    x      = LIM_INF + entero * (LIM_SUP - LIM_INF) / (2^m - 1)
// ---------------------------------------------------------------------------
vector<double> decodificar(const Cromosoma& cromosoma) {
    vector<double> x(NUM_VARIABLES);
    double enteroMaximo = pow(2.0, BITS_POR_VARIABLE) - 1;

    for (int v = 0; v < NUM_VARIABLES; v++) {
        long long entero = 0;
        for (int i = v * BITS_POR_VARIABLE; i < (v + 1) * BITS_POR_VARIABLE; i++) {
            entero = entero * 2 + cromosoma[i];
        }
        x[v] = LIM_INF + entero * (LIM_SUP - LIM_INF) / enteroMaximo;
    }
    return x;
}

string cromosomaATexto(const Cromosoma& cromosoma) {
    string texto;
    for (int gen : cromosoma) {
        texto += (gen ? '1' : '0');
    }
    return texto;
}


// ---------------------------------------------------------------------------
// 6. EVALUACION
// ---------------------------------------------------------------------------
// Funcion objetivo f(x) = x1^2 + x2^2. Cada llamada cuenta como una evaluacion.
double funcionObjetivo(const vector<double>& x) {
    evaluaciones++;
    double suma = 0.0;
    for (double xi : x) {
        suma += xi * xi;
    }
    return suma;
}

// Funcion de aptitud: como se minimiza, a menor f(x) mayor aptitud.
// aptitud = 1 / (1 + f(x)), vale 1 en el optimo y tiende a 0 cuando f crece.
double aptitud(double fx) {
    return 1.0 / (1.0 + fx);
}

vector<double> evaluarPoblacion(const Poblacion& poblacion) {
    vector<double> fx;
    for (const Cromosoma& cromosoma : poblacion) {
        fx.push_back(funcionObjetivo(decodificar(cromosoma)));
    }
    return fx;
}


// ---------------------------------------------------------------------------
// 7. SELECCION POR TORNEO
//    Se eligen TAM_TORNEO individuos al azar y gana el de mayor aptitud.
// ---------------------------------------------------------------------------
int seleccionTorneo(const vector<double>& fx) {
    int ganador = aleatorioEntero(0, TAM_POBLACION - 1);
    for (int k = 1; k < TAM_TORNEO; k++) {
        int rival = aleatorioEntero(0, TAM_POBLACION - 1);
        if (aptitud(fx[rival]) > aptitud(fx[ganador])) {
            ganador = rival;
        }
    }
    return ganador;
}


// ---------------------------------------------------------------------------
// 8. CRUZA DE UN PUNTO
//    Con probabilidad PROB_CRUZA se elige un punto de corte y se intercambian
//    las colas de los padres; si no hay cruza los hijos son copias.
// ---------------------------------------------------------------------------
void cruzaUnPunto(const Cromosoma& padre1, const Cromosoma& padre2,
                  Cromosoma& hijo1, Cromosoma& hijo2) {
    hijo1 = padre1;
    hijo2 = padre2;
    if (aleatorioReal() < PROB_CRUZA) {
        int punto = aleatorioEntero(1, LONGITUD_CROMOSOMA - 1);
        for (int i = punto; i < LONGITUD_CROMOSOMA; i++) {
            hijo1[i] = padre2[i];
            hijo2[i] = padre1[i];
        }
    }
}


// ---------------------------------------------------------------------------
// 9. MUTACION POR INVERSION DE BIT
//    Cada gen cambia (0 <-> 1) con probabilidad PROB_MUTACION.
// ---------------------------------------------------------------------------
void mutacion(Cromosoma& cromosoma) {
    for (int i = 0; i < LONGITUD_CROMOSOMA; i++) {
        if (aleatorioReal() < PROB_MUTACION) {
            cromosoma[i] = 1 - cromosoma[i];
        }
    }
}


// ---------------------------------------------------------------------------
// 10. ELITISMO
//     Devuelve los indices de los n mejores individuos (menor f(x)).
// ---------------------------------------------------------------------------
vector<int> indicesMejores(const vector<double>& fx, int n) {
    vector<int> indices(fx.size());
    iota(indices.begin(), indices.end(), 0);
    partial_sort(indices.begin(), indices.begin() + n, indices.end(),
                 [&](int a, int b) { return fx[a] < fx[b]; });
    indices.resize(n);
    return indices;
}


// ---------------------------------------------------------------------------
// 11. CICLO PRINCIPAL DEL AG (una ejecucion independiente)
// ---------------------------------------------------------------------------
struct Resultado {
    unsigned int semilla;
    Cromosoma mejor;
    vector<double> x;
    double fx;
    long long evaluaciones;
};

Resultado ejecutarAG(unsigned int semilla) {
    generador.seed(semilla);
    evaluaciones = 0;

    // Poblacion inicial aleatoria y su evaluacion
    Poblacion poblacion = inicializarPoblacion();
    vector<double> fx = evaluarPoblacion(poblacion);

    for (int generacion = 1; generacion <= NUM_GENERACIONES; generacion++) {
        Poblacion nuevaPoblacion;
        vector<double> fxNueva;

        // Elitismo: los mejores pasan sin cambios (ya estan evaluados)
        for (int i : indicesMejores(fx, NUM_ELITES)) {
            nuevaPoblacion.push_back(poblacion[i]);
            fxNueva.push_back(fx[i]);
        }

        // Seleccion, cruza y mutacion hasta completar la poblacion
        Poblacion hijos;
        while (nuevaPoblacion.size() + hijos.size() < (size_t) TAM_POBLACION) {
            const Cromosoma& padre1 = poblacion[seleccionTorneo(fx)];
            const Cromosoma& padre2 = poblacion[seleccionTorneo(fx)];

            Cromosoma hijo1, hijo2;
            cruzaUnPunto(padre1, padre2, hijo1, hijo2);
            mutacion(hijo1);
            mutacion(hijo2);

            hijos.push_back(hijo1);
            if (nuevaPoblacion.size() + hijos.size() < (size_t) TAM_POBLACION) {
                hijos.push_back(hijo2);
            }
        }

        // Evaluacion de los hijos
        vector<double> fxHijos = evaluarPoblacion(hijos);

        // Reemplazo generacional: elites + hijos forman la nueva poblacion
        nuevaPoblacion.insert(nuevaPoblacion.end(), hijos.begin(), hijos.end());
        fxNueva.insert(fxNueva.end(), fxHijos.begin(), fxHijos.end());
        poblacion = nuevaPoblacion;
        fx = fxNueva;
    }

    int mejor = indicesMejores(fx, 1)[0];
    return {semilla, poblacion[mejor], decodificar(poblacion[mejor]), fx[mejor], evaluaciones};
}


// ---------------------------------------------------------------------------
// 12. EXPERIMENTO: 30 ejecuciones independientes y reporte
// ---------------------------------------------------------------------------
void imprimirParametros() {
    cout << "=== Algoritmo Genetico: minimizar f(x) = x1^2 + x2^2 ===\n\n";
    cout << "Dominio de cada variable : [" << LIM_INF << ", " << LIM_SUP << "]\n";
    cout << "Precision                : " << DECIMALES << " decimales\n";
    cout << "Bits por variable        : " << BITS_POR_VARIABLE << "\n";
    cout << "Longitud del cromosoma   : " << LONGITUD_CROMOSOMA << " genes\n";
    cout << "Tamano de poblacion      : " << TAM_POBLACION << "\n";
    cout << "Generaciones             : " << NUM_GENERACIONES << "\n";
    cout << "Seleccion                : torneo de " << TAM_TORNEO << "\n";
    cout << "Cruza                    : un punto, Pc = " << PROB_CRUZA << "\n";
    cout << "Mutacion                 : inversion de bit, Pm = 1/L = "
         << fixed << setprecision(6) << PROB_MUTACION << "\n";
    cout << "Elitismo                 : " << NUM_ELITES << " individuos\n";
    cout << "Ejecuciones              : " << NUM_EJECUCIONES << "\n";
    cout << "Semilla base             : " << SEMILLA_BASE << "\n\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1) SEMILLA_BASE = (unsigned int) strtoul(argv[1], nullptr, 10);
    if (argc > 2) NUM_GENERACIONES = atoi(argv[2]);

    BITS_POR_VARIABLE = calcularBitsPorVariable(LIM_INF, LIM_SUP, DECIMALES);
    LONGITUD_CROMOSOMA = NUM_VARIABLES * BITS_POR_VARIABLE;
    PROB_MUTACION = 1.0 / LONGITUD_CROMOSOMA;

    imprimirParametros();

    vector<Resultado> resultados;
    for (int e = 0; e < NUM_EJECUCIONES; e++) {
        resultados.push_back(ejecutarAG(SEMILLA_BASE + e));
    }

    // Tabla de resultados en consola
    cout << left << setw(10) << "Ejecucion" << setw(10) << "Semilla"
         << right << setw(14) << "x_1" << setw(14) << "x_2"
         << setw(16) << "f(x)" << setw(14) << "Evaluaciones" << "\n";
    cout << string(78, '-') << "\n";
    for (int e = 0; e < NUM_EJECUCIONES; e++) {
        const Resultado& r = resultados[e];
        cout << left << setw(10) << e + 1 << setw(10) << r.semilla << right
             << fixed << setprecision(6) << setw(14) << r.x[0] << setw(14) << r.x[1]
             << scientific << setw(16) << r.fx
             << setw(14) << r.evaluaciones << "\n";
    }

    // Estadisticas de f(x) sobre las 30 ejecuciones
    double mejor = resultados[0].fx, peor = resultados[0].fx, suma = 0.0;
    int alcanzanOptimo = 0;
    for (const Resultado& r : resultados) {
        mejor = min(mejor, r.fx);
        peor = max(peor, r.fx);
        suma += r.fx;
        // Coincide con el optimo si ambas variables valen 0 a 6 decimales
        if (fabs(r.x[0]) < 0.5e-6 && fabs(r.x[1]) < 0.5e-6) alcanzanOptimo++;
    }
    double media = suma / NUM_EJECUCIONES;
    double sumaCuadrados = 0.0;
    for (const Resultado& r : resultados) {
        sumaCuadrados += (r.fx - media) * (r.fx - media);
    }
    double desviacion = sqrt(sumaCuadrados / NUM_EJECUCIONES);

    cout << "\nEstadisticas de f(x) en " << NUM_EJECUCIONES << " ejecuciones\n";
    cout << scientific << setprecision(6);
    cout << "  Mejor     : " << mejor << "\n";
    cout << "  Peor      : " << peor << "\n";
    cout << "  Media     : " << media << "\n";
    cout << "  Desv. est.: " << desviacion << "\n";
    cout << "  Ejecuciones con x1 = x2 = 0.000000: "
         << alcanzanOptimo << " de " << NUM_EJECUCIONES << "\n";

    // Resultados en CSV para el reporte
    ofstream archivo("resultados.csv");
    archivo << "ejecucion,semilla,x_1,x_2,f(x),evaluaciones,cromosoma\n";
    for (int e = 0; e < NUM_EJECUCIONES; e++) {
        const Resultado& r = resultados[e];
        archivo << e + 1 << "," << r.semilla << ","
                << fixed << setprecision(6) << r.x[0] << "," << r.x[1] << ","
                << scientific << r.fx << "," << r.evaluaciones << ","
                << cromosomaATexto(r.mejor) << "\n";
    }
    cout << "\nResultados guardados en resultados.csv\n";

    return 0;
}
