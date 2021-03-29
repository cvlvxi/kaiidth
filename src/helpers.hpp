#pragma once

#include "base.hpp"

void createGenericVkInstance(const char* appName, VkInstance& instance) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName,
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    appInfo.pEngineName = "None";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // glfw extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // Global Validation Layers
    createInfo.enabledLayerCount = 0;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance");
    }
    info("Successfully created a vulkan instance!");
}