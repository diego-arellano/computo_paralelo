#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

// Función para contar las ocurrencias de palabras en un libro
std::unordered_map<std::string, int> contarOcurrencias(std::ifstream &vocabulario, std::ifstream &libro) {
    std::unordered_map<std::string, int> contador;
    std::string palabra;

    // Inicializar el contador para cada palabra del vocabulario
    while (std::getline(vocabulario, palabra, ',')) {
        contador[palabra] = 0;
    }

    // Leer el libro y contar las ocurrencias de cada palabra
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
void guardarCSV(const std::unordered_map<std::string, int> &contador, const std::string &nombreLibro, const std::string &nombreArchivoSalida, const std::vector<std::string> &vocabulario) {
    std::ofstream archivoSalida(nombreArchivoSalida, std::ios::app); // Modo de apertura: append
    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return;
    }

    // Escribir el nombre del libro como primera columna
    archivoSalida << nombreLibro;

    // Escribir encabezados de columnas (palabras)
    for (const auto &palabra : vocabulario) {
        archivoSalida << "," << palabra;
    }

    archivoSalida << "\n";

    // Escribir datos
    archivoSalida << nombreLibro;
    for (const auto &palabra : vocabulario) {
        archivoSalida << "," << contador.at(palabra);
    }

    archivoSalida << "\n";

    archivoSalida.close();
}

int main() {
    // Nombre del archivo CSV de vocabulario
    std::string archivoVocabulario = "vocabulario.csv";

    // Nombres de los archivos CSV de libros
    std::vector<std::string> archivosLibros = {"libro1.csv", "libro2.csv", "libro3.csv", "libro4.csv", "libro5.csv", "libro6.csv"};

    // Leer el vocabulario
    std::ifstream archivoVocabularioStream(archivoVocabulario);
    std::vector<std::string> vocabulario;
    std::string palabra;
    while (std::getline(archivoVocabularioStream, palabra)) {
        vocabulario.push_back(palabra);
    }

    // Para cada libro, contar las ocurrencias y guardar en un archivo CSV
    for (const auto &archivoLibro : archivosLibros) {
        std::ifstream vocabulario(archivoVocabulario);
        std::ifstream libro(archivoLibro);

        if (!vocabulario.is_open() || !libro.is_open()) {
            std::cerr << "Error al abrir uno de los archivos de entrada." << std::endl;
            return 1;
        }

        // Obtener el nombre del libro sin la extensión
        std::string nombreLibro = archivoLibro.substr(0, archivoLibro.find_last_of("."));

        // Contar ocurrencias
        std::unordered_map<std::string, int> contador = contarOcurrencias(vocabulario, libro);

        // Guardar resultados en un archivo CSV
        guardarCSV(contador, nombreLibro, "resultados.csv", vocabulario);

        std::cout << "Proceso completado para " << nombreLibro << ". Resultados guardados en resultados.csv" << std::endl;

        // Cerrar archivos
        vocabulario.close();
        libro.close();
    }

    return 0;
}
