#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>

// Función para obtener el vocabulario desde un archivo CSV
std::vector<std::string> obtenerVocabulario(std::ifstream &vocabulario) {
    std::vector<std::string> palabras;
    std::string palabra;

    while (std::getline(vocabulario, palabra, ',')) {
        palabras.push_back(palabra);
    }

    return palabras;
}

// Función para contar las ocurrencias de palabras en un libro
std::map<std::string, int> contarOcurrencias(const std::vector<std::string> &vocabulario, std::ifstream &libro) {
    std::map<std::string, int> contador;
    std::string palabra;

    for (const auto &palabraVocabulario : vocabulario) {
        contador[palabraVocabulario] = 0;
    }

    std::string linea;
    while (std::getline(libro, linea, ',')) {
        std::istringstream iss(linea);
        while (iss >> palabra) {
            if (contador.find(palabra) != contador.end()) {
                contador[palabra]++;
            }
        }
    }

    return contador;
}

// Función para guardar las ocurrencias en un archivo CSV
void guardarCSV(const std::vector<std::string> &vocabulario, const std::vector<std::map<std::string, int>> &contadores, const std::vector<std::string> &nombresLibros, const std::string &nombreArchivoSalida) {
    std::ofstream archivoSalida(nombreArchivoSalida);
    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return;
    }

    // Escribir encabezados del vocabulario
    archivoSalida << "Libro";
    for (const auto &palabra : vocabulario) {
        archivoSalida << "," << palabra;
    }
    archivoSalida << "\n";

    // Escribir datos
    for (std::size_t i = 0; i < contadores.size(); ++i) {
        archivoSalida << nombresLibros[i];
        for (const auto &palabra : vocabulario) {
            archivoSalida << "," << contadores[i].at(palabra);
        }
        archivoSalida << "\n";
    }

    archivoSalida.close();
}

int main() {
    std::string archivoVocabulario = "vocabulario.csv";
    std::vector<std::string> archivosLibros = {"dickens_a_christmas_carol.txt", "dickens_a_tale_of_two_cities.txt", "dickens_oliver_twist.txt", "shakespeare_hamlet.txt", "shakespeare_romeo_juliet.txt", "shakespeare_the_merchant_of_venice.txt"};

    std::ifstream vocabulario(archivoVocabulario);
    if (!vocabulario.is_open()) {
        std::cerr << "Error al abrir el archivo de vocabulario." << std::endl;
        return 1;
    }

    // Obtener el vocabulario desde el archivo CSV
    std::vector<std::string> vocabularioPalabras = obtenerVocabulario(vocabulario);

    // Contadores para cada libro
    std::vector<std::map<std::string, int>> contadores;
    std::vector<std::string> nombresLibros;

    // Para cada libro, contar las ocurrencias y guardar en un mapa
    for (const auto &archivoLibro : archivosLibros) {
        std::ifstream libro(archivoLibro);
        if (!libro.is_open()) {
            std::cerr << "Error al abrir el archivo de libro." << std::endl;
            return 1;
        }

        // Obtener el nombre del libro sin la extensión
        std::string nombreLibro = archivoLibro.substr(0, archivoLibro.find_last_of("."));

        // Contar ocurrencias
        std::map<std::string, int> contador = contarOcurrencias(vocabularioPalabras, libro);

        // Agregar a los vectores
        contadores.push_back(contador);
        nombresLibros.push_back(nombreLibro);

        std::cout << "Proceso completado para " << nombreLibro << std::endl;

        libro.close();
    }

    // Guardar resultados en un archivo CSV
    guardarCSV(vocabularioPalabras, contadores, nombresLibros, "resultados.csv");

    vocabulario.close();

    return 0;
}

