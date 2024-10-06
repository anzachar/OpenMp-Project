#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <cmath>
#include <random>
#include <chrono> // time-based seeding

int main(int argc, char *argv[]) {
    
    // Checking for problematic number of arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <num_threads> <num_points>\n";
        return 1;
    }
    
    // Conversions
    int num_threads = std::atoi(argv[1]); // number of threads
    long long num_points = std::atoll(argv[2]); // number of points to generate

    omp_set_num_threads(num_threads); // setting number of threads to use in OpenMP

    long long inside_circle = 0; // counter to keep track of points inside the circle

    // Parallel Region
#pragma omp parallel 
    {
        // Creating a unique seed using current time plus thread number
        unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()) + omp_get_thread_num(); 
        long long local_inside_circle = 0; // local counter to keep track of points inside the circle
        std::mt19937 generator(seed); // Initializin generator with the unique seed
        std::uniform_real_distribution<double> distribution(0.0, 1.0); // creating uniform distribution from 0 to 1

#pragma omp for // Distributing iterations among the available threads
        for (long long i = 0; i < num_points; i++) {
            double x = distribution(generator); // random x 
            double y = distribution(generator); // random y
            
            if (x * x + y * y <= 1.0) { // if the radius is less than or equal to 1 (unit circle)
                local_inside_circle++; // incrementing local point counter
            }
        }

#pragma omp atomic // This operation is atomic to prevent race conditions
        inside_circle += local_inside_circle; // adding to global point counter
    }

    double pi_estimate = (4.0 * inside_circle) / num_points; // pi estimation

    // Print the estimated value of Pi
    std::cout << pi_estimate << std::endl;

    return 0;
}

