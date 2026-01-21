#include <oneapi/tbb.h>
#include <SFML/Graphics.hpp>
#include <atomic>
#include <chrono>
#include <complex>
#include <iostream>
#include <vector>
#include <fstream>

using Complex = std::complex<double>;

int mandelbrot(Complex const& c) {
    int i = 0;
    auto z = c;
    for (; i != 256 && norm(z) < 4.; ++i) {
        z = z * z + c;
    }
    return i;
}

auto to_color(int k) {
    return k < 256 ? sf::Color{static_cast<sf::Uint8>(10 * k), 0, 0}
                   : sf::Color::Black;
}

int main() {
    // Define the image dimensions
    const int display_width = 1200;
    const int display_height = 1200;

    Complex const top_left{-2.2, 1.5};
    Complex const lower_right{0.8, -1.5};
    auto const diff = lower_right - top_left;

    auto const delta_x = diff.real() / display_width;
    auto const delta_y = diff.imag() / display_height;

    sf::Image image;
    image.create(display_width, display_height);

    std::ofstream output_file("grain_size_times.txt"); // Creating an output file

    // Define list that stores the grain size, time taken, and number of tasks
    std::vector<std::tuple<int, double, int>> grain_size_times;

    // Loop over different grain sizes to test performance
    for (int grain_size = 1; grain_size <= display_height; grain_size < 10 ? ++grain_size : grain_size += 10) {  // We can of course change the grain size progression

        // Run each grain size 5 times to get an average time (the number of tasks will remain the same)
        double total_time = 0;
        std::atomic<int> task_count; // Define atomic to count tasks. Define here so that the scope is correct
        for (int i = 0; i < 5; ++i) {
            auto start = std::chrono::steady_clock::now();

            task_count = 0; // Reset task count for each run

            // Execute the parallel computation using the TBB package
            tbb::parallel_for(                                                                           // parallel_for to parallelize the loop
                tbb::blocked_range2d<int>(0, display_height, grain_size, 0, display_width, grain_size),  // blocked_range2d to specify the 2D range with grain size
                [&](tbb::blocked_range2d<int> const& r) {                                                // [&] to capture all variables by reference. blocked_range2d to specify the range
                    ++task_count;                                                                        // This will create a slight overhead, but it is necessary to count the tasks
                    for (int y = r.rows().begin(); y != r.rows().end(); ++y) {
                        for (int x = r.cols().begin(); x != r.cols().end(); ++x) {
                            auto k = mandelbrot(top_left + Complex{delta_x * x, delta_y * y});
                            image.setPixel(x, y, to_color(k));
                        }
                    }
            
                }, 
            tbb::simple_partitioner{}); // requested parameter

            auto end = std::chrono::steady_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            total_time += elapsed_time.count();
        }

        double average_time = total_time / 5.0;
        std::cout << "Grain size: " << grain_size << ", Average time taken: " << average_time << " microseconds, Tasks: " << task_count.load() << "\n";

        output_file << grain_size << " " << average_time << " " << task_count.load() << "\n";  // Saving to a file
    }

    // // No grain size specified, commented out to keep it in case the loop above does not work
    // // Execute the parallel computation using the TBB package
    // tbb::parallel_for(                                  // parallel_for to parallelize the loop
    //     tbb::blocked_range<int>(0, display_height),     // blocked_range for rows
    //     [&](tbb::blocked_range<int> const& r) {         // [&] to capture all variables by reference. blocked_range to specify the range
    //         for (int y = r.begin(); y != r.end(); ++y) {
    //             for (int x = 0; x != display_width; ++x) {
    //                 auto k = mandelbrot(top_left + Complex{delta_x * x, delta_y * y});
    //                 image.setPixel(x, y, to_color(k));
    //             }
    //         }
    //     });
    output_file.close();
    image.saveToFile("mandelbrot.png");

    (void) system("python3 /workspace/IMAPP-Comp-Sci-Project/Project_code/create_graphs.py"); // Call the Python script to create the plot. (void) to suppress unused return value warning
}
