# Kaiidth 

dif-tor heh smusma

## Requirements

- Vulkan: `Ensure VULKAN_SDK and runtime environment setup`

```
export PATH="$PATH:$VULKAN_SDK/bin"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$VULKAN_SDK/lib"
export VK_LAYER_PATH="$VULKAN_SDK/etc/explicit_layer.d"
```  

- glfw3
- glm: `sudo apt install libglm-dev`
- X: `sudo apt install libxcb1-dev xorg-dev`

## Windows

- Let's use the toolchain vcpkg for cmake when on windows 

### Find out the toolchain file command to use 


```
vcpkg install glfw3
vcpkg install glm
vcpkg integrate install
```

### Updating IDE with cmake command

Cmake settings
<img src="docs/clion.png"/>