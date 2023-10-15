#include <omp.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;


void noise_detection_serial(float** points, float epsilon, int min_samples, long long int size) {

    for (long long int i=0; i < size; i++) {
        if( points[i][2] == 0)
            cout << "Outlier: (" << points[i][0] << ", " << points[i][1] << ")\n";
    }

}

void noise_detection_paralelo(float** points, float epsilon, int min_samples, long long int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        if (points[i][2] == 0) {
            #pragma omp critical
            {
                cout << "Outlier: (" << points[i][0] << ", " << points[i][1] << ")\n";
            }
        }
    }
}

double distance(float* p1, float* p2) {
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    return sqrt(dx * dx + dy * dy);
}

// Función para encontrar los puntos vecinos de un punto dado dentro de un radio epsilon
std::vector<int> findNeighbors(float** points, int pointIndex, double eps, long long int size) {
    std::vector<int> neighbors;
    for (int i = 0; i < size; ++i) {
        if (i != pointIndex) {
            if (distance(points[pointIndex], points[i]) <= eps) {
                neighbors.push_back(i);
            }
        }
    }
    return neighbors;
}

// Función principal de DBSCAN
void dbscan_serial(float** points, double eps, int minPts, long long int size) {

    for (int i = 0; i < size; ++i) {
        if (points[i][2] != 0) {
            continue; // Punto ya visitado
        }

        std::vector<int> neighbors = findNeighbors(points, i, eps, size);

        if (neighbors.size() < minPts) {
            points[i][2] = 0; // Marcar como outlier
        } else {
            points[i][2] = 1; // Marcar como core point

            for (size_t j = 0; j < neighbors.size(); ++j) {
                int neighborIndex = neighbors[j];

                if (points[neighborIndex][2] == 0) {
                    points[neighborIndex][2] = 1;
                } else if (points[neighborIndex][2] == 1) {
                    std::vector<int> neighborNeighbors = findNeighbors(points, neighborIndex, eps, size);
                    if (neighborNeighbors.size() >= minPts) {
                        neighbors.insert(neighbors.end(), neighborNeighbors.begin(), neighborNeighbors.end());
                    }
                }
            }
        }
    }

    noise_detection_serial(points, eps, minPts, size);
}

// Función principal de DBSCAN paralelo
void dbscan_paralelo(float** points, double eps, int minPts, long long int size) {

    omp_set_num_threads(4);

    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        if (points[i][2] != 0) {
            continue; // Punto ya visitado
        }

        std::vector<int> neighbors;

        #pragma omp parallel for
        for (int j = 0; j < size; ++j) {
            if (i != j && distance(points[i], points[j]) <= eps) {
                neighbors.push_back(j);
            }
        }

        if (neighbors.size() < minPts) {
            points[i][2] = 0; // Marcar como ruido
        } else {
            points[i][2] = 1; // Asignar un nuevo clúster

            for (size_t j = 0; j < neighbors.size(); ++j) {
                int neighborIndex = neighbors[j];

                if (points[neighborIndex][2] == 0) {
                    points[neighborIndex][2] = 1;
                } 
            }
        }
    }

    // Llamar a detectOutliers para identificar outliers (ruido)
    noise_detection_paralelo(points, eps, minPts, size);
}

void load_CSV(string file_name, float** points, long long int size) {
    ifstream in(file_name);
    if (!in) {
        cerr << "Couldn't read file: " << file_name << "\n";
    }
    
    long long int point_number = 0; 
    while (!in.eof() && (point_number < size)) {
        char* line = new char[12];
        streamsize row_size = 12;
        in.read(line, row_size);
        string row = line;
        //cout << stof(row.substr(0, 5)) << " - " << stof(row.substr(6, 5)) << "\n";
        points[point_number][0] = stof(row.substr(0, 5));
        points[point_number][1] = stof(row.substr(6, 5));
        point_number++;
    } 
}

void save_to_CSV(string file_name, float** points, long long int size) {
    fstream fout;
    fout.open(file_name, ios::out);
    for (long long int i = 0; i < size; i++) {
        fout << points[i][0] << ","
             << points[i][1] << ","
             << points[i][2] << "\n";
    }
}

int main(int argc, char** argv) {

    const float epsilon = 0.02;
    const int min_samples = 40;
    int size = atoi(argv[1]);
    const string input_file_name = "4000_data.csv";
    const string output_file_name = to_string(size)+"_results.csv";    
    float** points = new float*[size];

    for(long long int i = 0; i < size; i++) {
        points[i] = new float[3]{0.0, 0.0, 0.0}; 
        // index 0: position x
        // index 1: position y 
        // index 2: 0 for noise point, 1 for core point
    }

    //float points[13][3] = {{-100, -100, 0.0}, {-101, -101, 0.0}, {-100.5, -100.5, 0.0}, {-99.9, -99.9, 0.0}, {1.2, 2.2, 0.0}, {1.0, 2.0, 0.0}, {1.5, 2.5, 0.0}, {2.0, 3.0, 0.0}, {8.0, 8.0, 0.0}, {8.5, 7.5, 0.0}, {9.0, 8.5, 0.0}, {9.2, 8.5, 0.0}, {100, 100, 0.0}};

    load_CSV(input_file_name, points, size);
    
    dbscan_paralelo(points, epsilon, min_samples, size); 
        
    save_to_CSV(output_file_name, points, size);

    for(long long int i = 0; i < size; i++) {
        delete[] points[i];
    }
    delete[] points;
    return 0;
}