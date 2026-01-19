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
    const int display_width = 800;
    const int display_height = 800;

    Complex const top_left{-2.2, 1.5};
    Complex const lower_right{0.8, -1.5};
    auto const diff = lower_right - top_left;

    auto const delta_x = diff.real() / display_width;
    auto const delta_y = diff.imag() / display_height;

    sf::Image image;
    image.create(display_width, display_height);

    std::ofstream output_file("grain_size_times.txt"); // Creating an output file

    // Define list that stores the grain size and the corresponding time taken
    std::vector<std::pair<int, double>> grain_size_times;

    // Loop over different grain sizes to test performance
    for (int grain_size = 1; grain_size < display_height; grain_size += 10) {  // We can of course change the grain size progression
        auto start = std::chrono::steady_clock::now();

        // Execute the parallel computation using the TBB package
        tbb::parallel_for(                                                                           // parallel_for to parallelize the loop
            tbb::blocked_range2d<int>(0, display_height, grain_size, 0, display_width, grain_size),  // blocked_range2d to specify the 2D range with grain size
            [&](tbb::blocked_range2d<int> const& r) {                                                // [&] to capture all variables by reference. blocked_range2d to specify the range
                for (int y = r.rows().begin(); y != r.rows().end(); ++y) {
                    for (int x = r.cols().begin(); x != r.cols().end(); ++x) {
                        auto k = mandelbrot(top_left + Complex{delta_x * x, delta_y * y});
                        image.setPixel(x, y, to_color(k));
                    }
                }
		
            }, 
	    tbb::simple_partitioner{}); // requested parameter

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        grain_size_times.emplace_back(grain_size, elapsed.count());
        std::cout << "Grain size: " << grain_size << ", Time taken: " << elapsed.count() << " seconds\n";
	
	output_file << grain_size << " " << elapsed.count() << "\n";  // Saving to a file
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
}
