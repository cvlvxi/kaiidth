#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <GLFW/glfw3.h>


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return 0;
}