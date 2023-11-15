#include <mpi.h>
#include <iostream>

using namespace std;

int main (int argc, char *argv[]) {
    // Verifica si se proporcionaron suficientes argumentos
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <archivo1> <archivo2> ... <archivon> <vocabulario.txt> <num_procesos>" << std::endl;
        return 1; // Termina el programa con un código de error
    }

    // Obtén los nombres de los archivos de palabras en un array
    std::vector<std::string> archivosPalabras;
    for (int i = 1; i < argc - 2; ++i) {
        archivosPalabras.push_back(argv[i]);
    }
    // Obtén el nombre del archivo de vocabulario
    std::string archivoVocabulario = argv[argc - 2];
    // Obtén el número de procesos
    int numProcesos = std::stoi(argv[argc - 1]);

    return 0;
}
