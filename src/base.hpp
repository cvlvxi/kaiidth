#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <optional>
#include "utils.hpp"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
};


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
    std::optional<uint32_t> graphicsFamily;
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

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                info("Success: Found graphicsFamily queue");
                indices.graphicsFamily = i;
            }
            i++;
        }
        return indices;
    }

    static bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               findQueueFamilies(device).graphicsFamily.has_value();
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
        for (const auto &device: devices) {
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

