#pragma once
#include "base.hpp"
#include "helpers.hpp"

class HelloWorldApplication : public BaseApplication {
private:
    void createInstance() override {
        createGenericVkInstance("Hello World", instance_, validationLayers_);
    }
};