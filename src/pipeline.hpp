#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include "helpers.hpp"

class BaseApplication;

struct ShaderModules {
    std::vector<VkShaderModule> vertShaders;
    std::vector<VkShaderModule> fragShaders;
};

class BasePipeline {
public:
    ShaderModules _shaderModules;
    BaseApplication& _app;

    explicit BasePipeline(BaseApplication& app) : _app(app) {}
    void cleanup();
    void addShader(const std::string& filename, bool isVert);

private:
    static VkShaderModule createShaderModule(BaseApplication &app, const std::vector<char>& code);

};