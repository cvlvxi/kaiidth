cmake_minimum_required(VERSION 3.16)

project(kaiidth)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)
find_package(glm REQUIRED)

add_executable(kaiidth src/main.cpp)

target_link_libraries(kaiidth glfw Vulkan::Vulkan)


