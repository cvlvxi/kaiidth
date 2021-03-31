#pragma once
#include "base.hpp"
#include "helpers.hpp"

class HelloWorldApplication : public BaseApplication {

private:
    void getRequiredExtensions() override {
         getGenericRequiredExtensions(this);
    }

    void createInstance() override {
        createGenericVkInstance("Hello World", this);
    }

    void createGraphicsPipeline() override {
        addShader(this, "shaders/001_triangle.vert.spv", true);
        addShader(this, "shaders/001_triangle.frag.spv", true);
    }

};