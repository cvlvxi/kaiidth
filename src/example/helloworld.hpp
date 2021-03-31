#pragma once
#include "helpers.hpp"
#include "pipeline.hpp"
#include "base.hpp"

class HelloWorldApplication : public BaseApplication {

private:
    void getRequiredExtensions() override {
         getGenericRequiredExtensions(this);
    }

    void createInstance() override {
        createGenericVkInstance("Hello World", this);
    }

    void createGraphicsPipelines() override {
        BasePipeline p = BasePipeline(*this);

        p.addShader("shaders/001_triangle.vert.spv", true);
        p.addShader("shaders/001_triangle.frag.spv", false);

        _pipelines.push_back(p);


//        addShader(this, "shaders/001_triangle.vert.spv", true);
//        addShader(this, "shaders/001_triangle.frag.spv", false);
//
//        // How to load stuff into the buffers in shaders
//        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//        vertexInputInfo.vertexBindingDescriptionCount = 0;
//        vertexInputInfo.pVertexBindingDescriptions = nullptr;
//
//        vertexInputInfo.vertexAttributeDescriptionCount = 0;
//        vertexInputInfo.pVertexAttributeDescriptions = nullptr;
//
//        // What kind of geometry
//        VkPipelineInputAssemblyStateCreateInfo vertexInputAssembly{};
//        vertexInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//        vertexInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//        vertexInputAssembly.primitiveRestartEnable = VK_FALSE;
//
//        // Viewports
//        VkViewport viewport{};
//        viewport.x = 0.0f;
//        viewport.y = 0.0f;
//        viewport.width = (float) _swapChainExtent.width;
//        viewport.height = (float) _swapChainExtent.height;
//        viewport.minDepth = 0.0f;
//        viewport.maxDepth = 1.0f;

    }

};