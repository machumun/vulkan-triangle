//
//  Debug.hpp
//  command20220406
//
//  Created by Hidesato on 2022/04/07.
//

#ifndef Debug_hpp
#define Debug_hpp

#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class Validation{
    
public:
    bool CheckValidationLayerSupport();
    
    void ValidationCheck();
    
    // 検証レイヤーが有効になっているかに基づいて拡張機能のリストを返す関数
    std::vector<const char*> GetRequiredExtensions();
    
    // vulkanSDKが提供する標準の診断レイヤーを有効にして初期化
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    
#ifdef PLATFORM_WINDOWS
    const char* platformName = "windows";
#elif defined PLATFORM_MACOS
    const char* platformName = "macos";
#else
    const char* platformName = "else";
#endif

    
    // なんだこれは..? 戻り値をマクロで挟んでいる？コンパイラに指示をだすマクロ??
    // 第一引数はメッセージの重大度。フラグで。
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:診断メッセージ
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:リソース作成などの情報メッセージ
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:ワーニングメッセージ
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:クラッシュを引き起こす可能性がある
    
    // messageTypeには次の値
    // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:仕様や性能とは関係ないイベント
    // VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:仕様に違反する、または間違いの可能性
    // VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:Vulkanの非最適な使用
    
    // pCallbackDataはメッセージ自体の詳細を含む構造体の参照
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        
        // 受け取ったメーセージが特定のレベルのものと比較して良いか悪いか判断する。
        if(messageSeverity>=VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){}
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    
    // デバッグメッセンジャーの作成
    VkResult CreateDebugUtilsMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    
    // デバッグメッセンジャーの破棄.debugMessengerクラスこっちはなぜポインタじゃない?
    void DestroyDebugUtilsMessengerEXT(VkInstance, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT);
    
};

#endif /* Debug_hpp */


