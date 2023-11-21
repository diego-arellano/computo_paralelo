#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include "mpi.h"

std::vector<std::string> obtenerVocabulario(std::ifstream &vocabulario) {
    std::vector<std::string> palabras;
    std::string palabra;

    while (std::getline(vocabulario, palabra, ',')) {
        palabras.push_back(palabra);
    }

    return palabras;
}

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

void guardarCSV(const std::vector<std::string> &vocabulario, const std::map<std::string, int> &contador, const std::string &nombreLibro, const std::string &nombreArchivoSalida) {
    std::ofstream archivoSalida(nombreArchivoSalida, std::ios_base::app);
    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return;
    }

    // Escribir datos
    archivoSalida << nombreLibro;
    for (const auto &palabra : vocabulario) {
        archivoSalida << "," << contador.at(palabra);
    }
    archivoSalida << "\n";

    archivoSalida.close();
}

void combinarResultados(const std::vector<std::string> &vocabulario, const std::string &nombreArchivoParcial, const std::string &nombreArchivoSalida) {
    MPI_Status status;
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_rank == 0) {
        std::ofstream archivoSalida(nombreArchivoSalida);
        if (!archivoSalida.is_open()) {
            std::cerr << "Error al abrir el archivo de salida final." << std::endl;
            return;
        }

        archivoSalida << "Libro";
        for (const auto &palabra : vocabulario) {
            archivoSalida << "," << palabra;
        }
        archivoSalida << "\n";

        // Proceso 0 lee su archivo parcial y agrega al archivo final
        std::ifstream archivoParcial(nombreArchivoParcial);
        if (archivoParcial.is_open()) {
            std::string linea;
            while (std::getline(archivoParcial, linea)) {
                archivoSalida << linea << "\n";
            }
            archivoParcial.close();
        } else {
            std::cerr << "Error al abrir el archivo parcial del proceso 0." << std::endl;
        }

        // Recibir los resultados de los otros procesos
        for (int i = 1; i < world_size; ++i) {
            std::vector<std::string> resultadosParciales;

            MPI_Recv(&resultadosParciales[0], resultadosParciales.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);

            for (const auto &linea : resultadosParciales) {
                archivoSalida << linea << "\n";
            }
        }

        archivoSalida.close();
    } else {
        // Otros procesos envían sus resultados al proceso 0
        std::ifstream archivoParcial(nombreArchivoParcial);
        if (archivoParcial.is_open()) {
            std::string linea;
            std::vector<std::string> resultadosParciales;

            while (std::getline(archivoParcial, linea)) {
                resultadosParciales.push_back(linea);
            }

            MPI_Send(&resultadosParciales[0], resultadosParciales.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);

            archivoParcial.close();
        } else {
            std::cerr << "Error al abrir el archivo parcial del proceso " << world_rank << "." << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    std::string archivoVocabulario = "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\vocabulario.csv";
    std::vector<std::string> archivosLibros = {"C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\dickens_a_christmas_carol.txt", "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\dickens_a_tale_of_two_cities.txt", "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\dickens_oliver_twist.txt", 
    "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\shakespeare_hamlet.txt", "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\shakespeare_romeo_juliet.txt", 
    "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\shakespeare_the_merchant_of_venice.txt"};

    std::ifstream vocabulario(archivoVocabulario);
    
    if (!vocabulario.is_open()) {
        std::cerr << "Error al abrir el archivo de vocabulario." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::vector<std::string> vocabularioPalabras;
    if (world_rank == 0) {
        vocabularioPalabras = obtenerVocabulario(vocabulario);
    }

    MPI_Bcast(vocabularioPalabras.data(), vocabularioPalabras.size(), MPI_CHAR, 0, MPI_COMM_WORLD);


    for (int i = world_rank; i < archivosLibros.size(); i += world_size) {
        std::ifstream libro(archivosLibros[i]);
        
        if (!libro.is_open()) {
            std::cerr << "Error al abrir el archivo de libro." << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        } 

        std::string nombreLibro = archivosLibros[i].substr(0, archivosLibros[i].find_last_of("."));
       
        std::map<std::string, int> contador = contarOcurrencias(vocabularioPalabras, libro);

        std::string nombreArchivoParcial = "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\resultados_parciales_" + std::to_string(world_rank) + ".csv";

        // Cada proceso guarda su resultado en un archivo parcial
        guardarCSV(vocabularioPalabras, contador, nombreLibro, nombreArchivoParcial);

        std::cout << "Proceso " << world_rank << " completado para " << nombreLibro << std::endl;

        libro.close();
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Combina los resultados parciales en un solo archivo CSV
    combinarResultados(vocabularioPalabras, "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\resultados_parciales_" + std::to_string(world_rank) + ".csv",
                       "C:\\Users\\Diego Arellano\\Desktop\\ITAM\\com paralelo\\bag_words\\resultados.csv");

    MPI_Finalize();

    return 0;
}
