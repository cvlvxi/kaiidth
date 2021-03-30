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

## OSX

Install vulkan and ensure the following is set in your profile

```
# Vulkan
VULKAN_SDK="/path/to/vulkansdk/macOS"
export VULKAN_SDK
PATH="$PATH:$VULKAN_SDK/bin"
export PATH
DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:${DYLD_LIBRARY_PATH}"
export DYLD_LIBRARY_PATH
VK_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"
export VK_LAYER_PATH
VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
export VK_ICD_FILENAMES
```

## Errors vk
`export VK_LOADER_DEBUG=all` to see issues with vulkan e.g. failed ot create instance