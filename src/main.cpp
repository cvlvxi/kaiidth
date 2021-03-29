#include "utils.hpp"
#include <stdexcept>
#include <cstdlib>
#include "example/helloworld.hpp"

int main() {
    HelloWorldApplication app;
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}