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
};