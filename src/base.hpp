#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include "utils.hpp"


class BaseApplication {
public:
    //////////////////////////////////////////////////////////
    // Public Member Variables
    //////////////////////////////////////////////////////////
    const uint32_t WIDTH_ = 800;
    const uint32_t HEIGHT_ = 600;
    GLFWwindow *window_{};
    VkInstance instance_{};
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    std::vector<const char *> validationLayers_;
    std::vector<const char *> extensions_;

#if NDEBUG
    bool enableValidation_ = false;
#else
    bool enableValidation_ = true;
#endif

    //////////////////////////////////////////////////////////
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    static bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
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
        getRequiredExtensions();
        createInstance();
        pickPhysicalDevice();
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

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("Error: No GPUs found");
        }
        info("Success: Found {} physical devices", deviceCount);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

        // Check devices
        for (const auto& device: devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice_ = device;
                info("Success: Found suitable device");
                break;
            }
        }

        if (physicalDevice_ == VK_NULL_HANDLE) {
            throw std::runtime_error("Error: Failed to find a suitable GPU.");
        }

    }



    //////////////////////////////////////////////////////////
    // Virtual Methods
    //////////////////////////////////////////////////////////
    virtual void getRequiredExtensions() = 0;

    virtual void createInstance() = 0;

};

