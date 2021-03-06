cmake_minimum_required(VERSION 3.16)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

project(kaiidth)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(WARNING_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(extern/spdlog)

include_directories(src)

add_executable(kaiidth src/main.cpp src/pipeline.hpp src/helpers.cpp src/pipeline.cpp src/base.cpp)

if (${APPLE})
    set(glm_lib glm)
else ()
    set(glm_lib glm::glm)
endif ()

target_link_libraries(
    kaiidth
    glfw
    Vulkan::Vulkan
    ${glm_lib}
    spdlog
)


