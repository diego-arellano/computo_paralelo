#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>



// Función para contar las ocurrencias de palabras en el libro
std::unordered_map<std::string, int> contarOcurrencias(std::ifstream &vocabulario, std::ifstream &libro) {
    std::unordered_map<std::string, int> contador;
    std::string palabra;

    // Leer el vocabulario
    while (std::getline(vocabulario, palabra, ',')) {
        contador[palabra] = 0; // Inicializar el contador para cada palabra del vocabulario
    }

    // Leer el libro
    std::string linea;
    while (std::getline(libro, linea, ',')) {
        std::istringstream iss(linea);
        while (iss >> palabra) {
            // Si la palabra está en el vocabulario, incrementar el contador
            if (contador.find(palabra) != contador.end()) {
                contador[palabra]++;
            }
        }
    }

    return contador;
}

// Función para guardar las ocurrencias en un archivo CSV
void guardarCSV(const std::unordered_map<std::string, int> &contador, const std::string &nombreArchivoSalida) {
    std::ofstream archivoSalida(nombreArchivoSalida);
    if (!archivoSalida.is_open()) {
        std::cerr << "Error" << std::endl;
        return;
    }

    // Escribir encabezado
    archivoSalida << "Palabra, Ocurrencias\n";

    // Escribir datos
    for (const auto &par : contador) {
        archivoSalida << par.first << "," << par.second << "\n";
    }

    archivoSalida.close();
}

int main() {
    // Nombre de los archivos CSV de entrada y salida
    std::string archivoVocabulario = "vocabulario.csv";
    std::string archivoLibro = "dickens_a_christmas_carol.txt";
    std::string archivoSalida = "ocurrencias.csv";

    // Abrir archivos de entrada
    std::ifstream vocabulario(archivoVocabulario);
    std::ifstream libro(archivoLibro);

    if (!vocabulario.is_open()) {
        std::cerr << "Error al abrir el archivo de vocabulario." << std::endl;
    }

    if (!libro.is_open()) {
        std::cerr << "Error al abrir el archivo del libro." << std::endl;
    }

    // Contar ocurrencias
    std::unordered_map<std::string, int> contador = contarOcurrencias(vocabulario, libro);

    // Guardar resultados en archivo CSV
    guardarCSV(contador, archivoSalida);

    std::cout << "Proceso completado. Resultados guardados en " << archivoSalida << std::endl;

    // Cerrar archivos
    vocabulario.close();
    libro.close();

    return 0;
}
