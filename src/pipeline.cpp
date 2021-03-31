#include "pipeline.hpp"
#include "base.hpp"
#include "log.hpp"

void BasePipeline::addShader(const std::string &filename, bool isVert) {
    if (isVert) {
        _shaderModules.vertShaders.push_back(createShaderModule(_app, readFile(filename)));
    } else {
        _shaderModules.fragShaders.push_back(createShaderModule(_app, readFile(filename)));
    }
}

VkShaderModule BasePipeline::createShaderModule(BaseApplication &app, const std::vector<char> &code) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    // This needs a uint32_t* rather than char *
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(app._device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}


void BasePipeline::cleanup() {
    info("Clean up: Pipeline");
    info("Clean up: Shaders");
    for (auto vertShaderModule : _shaderModules.vertShaders) {
        vkDestroyShaderModule(_app._device, vertShaderModule, nullptr);
    }
    for (auto fragShaderModule : _shaderModules.fragShaders) {
        vkDestroyShaderModule(_app._device, fragShaderModule, nullptr);
    }
}
