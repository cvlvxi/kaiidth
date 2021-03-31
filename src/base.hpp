#pragma once

#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <optional>
#include <set>
#include "pipeline.hpp"


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
    std::vector<BasePipeline> _pipelines;
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
    }

    ~BaseApplication();

private:
    //////////////////////////////////////////////////////////
    // Private Member Variables
    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createImageViews();

    void createLogicalDevice();

    void createSurface();

    void createSwapChain();

    void findQueueFamilies(VkPhysicalDevice device);

    void initWindow();

    void initVulkan();

    bool isDeviceSuitable(VkPhysicalDevice device);

    void mainLoop() const;

    void pickPhysicalDevice();

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    //////////////////////////////////////////////////////////
    // Virtual Methods
    //////////////////////////////////////////////////////////
    virtual void getRequiredExtensions() = 0;

    virtual void createInstance() = 0;

    virtual void createGraphicsPipelines() = 0;

};

