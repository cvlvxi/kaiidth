#include "helpers.hpp"

#include <cstring>
#include <fstream>
#include "log.hpp"
#include "base.hpp"

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    info("\t Opening file {}", filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Failed to open file. Did you compile the shader?");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

bool checkValidationLayerSupport(BaseApplication *app) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const char *layerName : app->_validationLayers) {
        bool layerFound = false;
        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}

void getGenericRequiredExtensions(BaseApplication *app) {
    // glfw extensions
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (app->enableValidation_) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    app->_extensions = extensions;
}

void createGenericVkInstance(const char *appName, BaseApplication *app) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    appInfo.pEngineName = "None";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;


    if (app->enableValidation_ && !checkValidationLayerSupport(app)) {
        throw std::runtime_error("Error: Validation Layers requested but not available");
    }

    if (app->enableValidation_) {
        info("Success: Checked validation layer support");
        createInfo.enabledLayerCount = static_cast<uint32_t>(app->_validationLayers.size());
        createInfo.ppEnabledLayerNames = app->_validationLayers.data();
        info("\t Number of validation layers present: {}", app->_validationLayers.size());
        for (size_t i = 0; i < app->_validationLayers.size(); ++i) {
            info("\t Success: Enabling validation layer {}", app->_validationLayers[i]);
        }
    } else {
        // No layers
        createInfo.enabledLayerCount = 0;
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(app->_extensions.size());
    createInfo.ppEnabledExtensionNames = app->_extensions.data();
    // Global Validation Layers
    createInfo.enabledLayerCount = 0;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &app->_instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error: Failed to create instance");
    }
    info("Success: Created a vulkan instance");


}


