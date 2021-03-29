#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "utils.hpp"


class BaseApplication {
public:
    //////////////////////////////////////////////////////////
    // Public Member Variables
    //////////////////////////////////////////////////////////
    const uint32_t WIDTH_ = 800;
    const uint32_t HEIGHT_ = 600;
    GLFWwindow* window_{};
    VkInstance instance_{};
    //////////////////////////////////////////////////////////
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    //////////////////////////////////////////////////////////
    // Private Member Variables
    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window_ = glfwCreateWindow(WIDTH_, HEIGHT_, "FIXME", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
    }


    void mainLoop() {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }


    //////////////////////////////////////////////////////////
    // Virtual Methods
    //////////////////////////////////////////////////////////
    virtual void createInstance() = 0;

};

