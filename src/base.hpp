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

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


class BaseApplication {
public:
    //////////////////////////////////////////////////////////
    // Public Member Variables
    //////////////////////////////////////////////////////////
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *_window{};
    QueueFamilyIndices _indices;
    std::vector<const char *> _validationLayers;
    std::vector<const char *> _extensions;
    std::vector<const char *> _deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;
    VkDevice _device{};
    VkExtent2D _swapChainExtent;
    VkFormat _swapChainImageFormat;
    VkInstance _instance{};
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkQueue _graphicsQueue{};
    VkQueue _presentQueue{};
    VkSurfaceKHR _surface{};                                  // Window Surface Integration from glfw
    VkSwapchainKHR _swapChain{};

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

private:
    //////////////////////////////////////////////////////////
    // Private Member Variables
    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t deviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, availableExtensions.data());

        // Check against declared wanted device extensions
        std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());
        for (const auto &extension: availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }
        return requiredExtensions.empty();
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                info("\t Choosing swap surface format {}", availableFormat.format);
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode : availablePresentModes) {
            // Triple Buffering
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                info("\t Choosing swap present mode {}", availablePresentMode);
                return availablePresentMode;
            }
        }
        info("\t Choosing swap present mode {}", VK_PRESENT_MODE_FIFO_KHR);
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            info("\t Choosing swap extent with width {} and height {}", capabilities.currentExtent.width,
                 capabilities.currentExtent.height);
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {WIDTH, HEIGHT};
            actualExtent.width = std::max(
                    capabilities.minImageExtent.width,
                    std::min(capabilities.maxImageExtent.width,
                             actualExtent.width));
            actualExtent.height = std::max(
                    capabilities.minImageExtent.height,
                    std::min(capabilities.maxImageExtent.height,
                             actualExtent.height));
            info("\t Choosing swap extent with width {} and height {}", actualExtent.width, actualExtent.height);
            return actualExtent;
        }
    }

    void cleanup() const {
        for (auto imageView : _swapChainImageViews) {
            vkDestroyImageView(_device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(_device, _swapChain, nullptr);
        vkDestroyDevice(_device, nullptr);
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyInstance(_instance, nullptr);
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void createImageViews() {
        _swapChainImageViews.resize(_swapChainImages.size());
        size_t idx = 0;
        for (auto &swapChainImage : _swapChainImages) {
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = swapChainImage;
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = _swapChainImageFormat;
            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount= 1;

            if (vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &_swapChainImageViews[idx]) != VK_SUCCESS) {
                throw std::runtime_error("Error: Could not create Image View");
            }
            info("Success: Created ImageView!");
            ++idx;
        }
    }

    void createLogicalDevice() {

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;

        info("\t Graphics Index: {} | Present Index: {}", _indices.graphicsFamily.value(),
             _indices.presentFamily.value());

        // Graphics Family Queue
        VkDeviceQueueCreateInfo gfxQueueCreateInfo{};
        gfxQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gfxQueueCreateInfo.queueFamilyIndex = _indices.graphicsFamily.value();
        gfxQueueCreateInfo.queueCount = 1;
        gfxQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(gfxQueueCreateInfo);

        // Present Family Queue
        VkDeviceQueueCreateInfo presentQueueCreateInfo{};
        gfxQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gfxQueueCreateInfo.queueFamilyIndex = _indices.presentFamily.value();
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
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = _deviceExtensions.data();
        for (const auto &extension : _deviceExtensions) {
            info("\t Success: Enabling device extension: {}", extension);
        }

        // Let's enable validation layers if set on device (even though not needed)
        if (enableValidation_) {
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = _validationLayers.data();
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS) {
            throw std::runtime_error("ERROR: failed to create logical device");
        }
        info("Success: Created the Logical Device");
        // FIXME: Is the graphics and present queue the same? This is ok but sometimes this will be different
        vkGetDeviceQueue(_device, _indices.graphicsFamily.value(), 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, _indices.presentFamily.value(), 0, &_presentQueue);
        info("Success: Got the graphics/present queue");
    }

    void createGraphicsPipeline() {

    }

    void createSurface() {
        if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) {
            throw std::runtime_error("ERROR: Could not create a window surface");
        }
        info("Success: Created the surface");
    }

    void createSwapChain() {
        // Check device first
        if (_device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(_device);
        }

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        // How many images in the swap chain?
        // + 1 means not waiting on driver to complete internal operations before getting another image to render to
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // Don't allow the imageCount to be greater than the max Image Count capability
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapChainCreateInfo{};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.pNext = nullptr;
        swapChainCreateInfo.flags = 0;
        swapChainCreateInfo.surface = _surface;
        swapChainCreateInfo.minImageCount = imageCount;
        swapChainCreateInfo.imageFormat = surfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = extent;
        swapChainCreateInfo.imageArrayLayers = 1; // Unless stereoscopic 3d stuff this will be 1
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // What kind of operations we'll use the images in swap chain for

        uint32_t queueFamilyIndices[] = {_indices.graphicsFamily.value(), _indices.presentFamily.value()};
        if (_indices.graphicsFamily.value() != _indices.presentFamily.value()) {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapChainCreateInfo.queueFamilyIndexCount = 0;
            swapChainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        // Transform the image in the swapChain? No in our case
        swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // Do we want to blend the alpha channel with other windows?
        // This says to ignore the alpha channel below (i.e. don't do this)
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        swapChainCreateInfo.presentMode = presentMode;
        // Only set this to false if you want to read pixels back and get predictable results
        swapChainCreateInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(_device, &swapChainCreateInfo, nullptr, &_swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Error: Could not create swap chain");
        }
        info("Success: Created the swap chain");

        // Retrieve the swap chain images
        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
        info("Success: Retrieved the swapChainImages");

        _swapChainImageFormat = surfaceFormat.format;
        _swapChainExtent = extent;
    }

    void findQueueFamilies(VkPhysicalDevice device) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        VkBool32 presentSupport = false;
        for (const auto &queueFamily : queueFamilies) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
            if (presentSupport) {
                info("Success: Found presentFamily queue indices");
                _indices.presentFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                info("Success: Found graphicsFamily queue indices");
                _indices.graphicsFamily = i;
                // Once we find the device with present and graphics lets get out?
                break;
            }
            i++;
        }
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        _window = glfwCreateWindow(WIDTH, HEIGHT, "FIXME", nullptr, nullptr);
    }

    void initVulkan() {
        getRequiredExtensions();
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createGraphicsPipeline();
    }


    bool isDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        findQueueFamilies(device);
        // See if it supports all the deviceExtensions we want
        bool deviceExtensionsSupported = checkDeviceExtensionSupport(device);

        // Check that the swapchain is ok
        bool swapChainAdequate = false;
        if (deviceExtensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            info("Success: Swapchain support is adequate");
        }
        return (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
                deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) && _indices.isComplete() &&
               deviceExtensionsSupported && swapChainAdequate;
    }

    void mainLoop() const {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("Error: No GPUs found");
        }
        info("Success: Found {} physical devices", deviceCount);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        // Check devices
        for (const auto &device: devices) {
            if (isDeviceSuitable(device)) {
                _physicalDevice = device;
                info("Success: Found suitable device");
                break;
            }
        }

        if (_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Error: Failed to find a suitable GPU.");
        }
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    //////////////////////////////////////////////////////////
    // Virtual Methods
    //////////////////////////////////////////////////////////
    virtual void getRequiredExtensions() = 0;

    virtual void createInstance() = 0;

};

