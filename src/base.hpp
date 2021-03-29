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

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value();
    }
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
    VkDevice device_{};
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    QueueFamilyIndices indices_;
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

    void findQueueFamilies(VkPhysicalDevice device) {
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
                indices_ = indices;
                break;
            }
            i++;
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        findQueueFamilies(device);
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indices_.isComplete();
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
        createLogicalDevice();
    }


    void mainLoop() {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyDevice(device_, nullptr);
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void createLogicalDevice() {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices_.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;

        // Let's enable validation layers if set on device (even though not needed)
        if (enableValidation_) {
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers_.size());
            deviceCreateInfo.ppEnabledLayerNames = validationLayers_.data();
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) {
            throw std::runtime_error("ERROR: failed to create logical device");
        }
        info("Success: Created the Logical Device");
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

