#pragma once
#include "base.hpp"
#include "helpers.hpp"

class HelloWorldApplication : public BaseApplication {
public:
    std::vector<VkShaderModule> _shaderModules;

    VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = code.size();
        // This needs a uint32_t* rather than char *
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(_device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
        return shaderModule;
    }
private:
    void getRequiredExtensions() override {
         getGenericRequiredExtensions(this);
    }

    void createInstance() override {
        createGenericVkInstance("Hello World", this);
    }

    void createGraphicsPipeline() override {
        VkShaderModule vertShaderModule = createShaderModule(readFile("shaders/001_triangle.vert.spv"));
        VkShaderModule fragShaderModule = createShaderModule(readFile("shaders/001_triangle.frag.spv"));
        _shaderModules.push_back(vertShaderModule);
        _shaderModules.push_back(fragShaderModule);
    }

protected:
    void cleanup() const override {
        info("Clean up: Helloworld");
        for (auto shaderModule : _shaderModules) {
            vkDestroyShaderModule(_device, shaderModule, nullptr);
        }
        BaseApplication::cleanup();
    }




};