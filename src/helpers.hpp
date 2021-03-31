#pragma once

#include <vector>
#include <string>

class BaseApplication;
std::vector<char> readFile(const std::string& filename);
bool checkValidationLayerSupport(BaseApplication *app);
void getGenericRequiredExtensions(BaseApplication *app);
void createGenericVkInstance(const char *appName, BaseApplication *app);


