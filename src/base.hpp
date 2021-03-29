#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <optional>
#include <set>
#include "utils.hpp"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
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
    QueueFamilyIndices indices_;
    VkDevice device_{};
    VkInstance instance_{};
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
    VkSurfaceKHR surface_;                                  // Window Surface Integration from glfw
    std::optional<uint32_t> graphicsFamily;
    std::vector<const char *> validationLayers_;
    std::vector<const char *> extensions_;
    std::vector<const char *> deviceExtensions_ = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#if NDEBUG
    bool enableValidation_ = false;
#else
    bool enableValidation_ = true;
#endif

    //////////////////////////////////////////////////////////
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t deviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, availableExtensions.data());

        // Check against declared wanted device extensions
        std::set<std::string> requiredExtensions(deviceExtensions_.begin(), deviceExtensions_.end());
        for (const auto &extension: availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }

    void findQueueFamilies(VkPhysicalDevice device) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        VkBool32 presentSupport = false;
        for (const auto &queueFamily : queueFamilies) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
            if (presentSupport) {
                info("Success: Found presentFamily queue indices");
                indices_.presentFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                info("Success: Found graphicsFamily queue indices");
                indices_.graphicsFamily = i;
                // Once we find the device with present and graphics lets get out?
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
        // See if it supports all the deviceExtensions we want
        bool deviceExtensionsSupported = checkDeviceExtensionSupport(device);
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indices_.isComplete() &&
               deviceExtensionsSupported;
    }

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
    void cleanup() const {
        vkDestroyDevice(device_, nullptr);
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        vkDestroyInstance(instance_, nullptr);
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void createLogicalDevice() {

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;

        info("\t Graphics Index: {} | Present Index: {}", indices_.graphicsFamily.value(),
             indices_.presentFamily.value());

        // Graphics Family Queue
        VkDeviceQueueCreateInfo gfxQueueCreateInfo{};
        gfxQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gfxQueueCreateInfo.queueFamilyIndex = indices_.graphicsFamily.value();
        gfxQueueCreateInfo.queueCount = 1;
        gfxQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(gfxQueueCreateInfo);

        // Present Family Queue
        VkDeviceQueueCreateInfo presentQueueCreateInfo{};
        gfxQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gfxQueueCreateInfo.queueFamilyIndex = indices_.presentFamily.value();
        gfxQueueCreateInfo.queueCount = 1;
        gfxQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(presentQueueCreateInfo);

        VkPhysicalDeviceFeatures deviceFeatures{};

        // Creating the Logical Device
        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        // Extensions
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions_.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions_.data();
        for (const auto& extension : deviceExtensions_) {
            info("\t Success: Enabling device extension: {}", extension);
        }

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
        // FIXME: Is the graphics and present queue the same? This is ok but sometimes this will be different
        vkGetDeviceQueue(device_, indices_.graphicsFamily.value(), 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices_.presentFamily.value(), 0, &presentQueue_);
        info("Success: Got the graphics/present queue");
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
            throw std::runtime_error("ERROR: Could not create a window surface");
        }
        info("Success: Created the surface");
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window_ = glfwCreateWindow(WIDTH_, HEIGHT_, "FIXME", nullptr, nullptr);
    }

    void initVulkan() {
        getRequiredExtensions();
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void mainLoop() const {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
        }
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

