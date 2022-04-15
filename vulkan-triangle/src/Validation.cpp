//
//  Debug.cpp
//  command20220406
//
//  Created by Hidesato on 2022/04/07.
//

#include "Validation.hpp"


bool Validation::CheckValidationLayerSupport(){
    uint32_t layerCount;
    
    // インスタンスレイヤーの数をlayerCountに保持
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    // 動的配列を確保
    std::vector<VkLayerProperties> availableLayers(layerCount);
    
    // 詳細のポインタを渡して参照をつくる
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    for(const char* layerName : validationLayers){
        bool layerFound = false;
        
        for(const auto& layerProperties : availableLayers){
            if(strcmp(layerName, layerProperties.layerName) == 0){
                layerFound = true;
                break;
            }
        }
        if(!layerFound){
            return false;
        }
    }

    return VK_TRUE;
}

void Validation::ValidationCheck(){
    if(enableValidationLayers && !CheckValidationLayerSupport()){
        throw std::runtime_error("");
    }
}

std::vector<const char*> Validation::GetRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    // glfwExtensionsの長さで初期化。
    std::vector<const char*> extensions(glfwExtensions,glfwExtensions + glfwExtensionCount);
    
    // 拡張機能リストの末尾にマクロでデバッグメッセンジャを追加
    // Molten用のインスタンス拡張を追加

//    extensions.push_back("VK_KHR_portability_subset");
//    VK_KHR_p
    extensions.push_back("VK_KHR_get_physical_device_properties2");
//    extensions.push_back("VK_EXT_metal_surface");
    if(enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    return extensions;
}

VkResult Validation::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Validation::DestroyDebugUtilsMessengerEXT(VkInstance instance,  const VkAllocationCallbacks* pAllocator,VkDebugUtilsMessengerEXT debugMessenger) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

