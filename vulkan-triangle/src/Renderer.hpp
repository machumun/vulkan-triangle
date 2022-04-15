//
//  Renderer.hpp
//  command20220406
//
//  Created by Hidesato on 2022/04/07.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#define VK_USE_PLATFORM_MACOS_MVK
//#define VK_USE_PLATFORM_WIN32_KHR
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <cstdint> 
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

#include "Validation.hpp"
#include "ShaderLoader.hpp"

class Renderer{

public:
    void CreateInstance();
    void DestoroyInstance();
    
    // 物理デバイスの選択
    void PickPhysicalDevice();
    
    // 論理デバイスの作成
    void CreateLogicalDevice();
    void DestroyDevice();

    // サーフェス作成
    void CreateSurface(GLFWwindow*);
    void DestroySurface();
    
    Validation userValidation;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
    void SetupDebugMessenger();
    
    // キューファミリ検索のための構造体
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
               return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    
    bool IsDeviceSuitable(VkPhysicalDevice);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice);
    
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice);

    // スワップチェーンサポートの詳細を紹介するための構造体
    // 1.基本的な表面機能　スワップチェーンに放り込める画像の最大最小値
    // 2.表面フォーマット
    // 3.利用可能なプレゼンテーションモード?
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D ChooseSwapExtent(GLFWwindow*, const VkSurfaceCapabilitiesKHR&);

    // スワップチェーン作成
    void CreateSwapChain(GLFWwindow*);
    void DestroySwapChain();

    // イメージビュー作成
    void CreateImageViews();
    void DestroyImageViews();

    // レンダリングパス作成
    void CreateRenderPass();
    void DestroyRenderPass();
    
    // グラフィックスパイプライン作成
    void CreateGraphicsPipeline();
    void DestoyGraphicsPipeline();
    
    // フレームバッファ作成
    void CreateFrameBuffers();
    void DestroyFrameBuffers();
    
    // コマンドプール作成
    void CreateCommandPool();
    void DestroyCommandPool();
    
    // コマンドバッファ作成
    void CreateCommandBuffer();
    void DestroyCommandBuffer();
    
    // コマンドバッファの記録用関数
    void RecordCommandBuffer(VkCommandBuffer,uint32_t);
    
    // ドローフレーム
    void DrawFrame();
    
    // 同期オブジェクトの作成
    void CreateSyncObjects();
    void DestroySyncObjects();
    
    // 論理デバイスが終わってからクリーンアップをするための関数
    void DeviceWaitIdle(){
        vkDeviceWaitIdle(_device);
    }
    
    // create総まとめ関数
    void Create(GLFWwindow*);
    void Destroy();
    
    ShaderLoader shaderLoader;

#ifdef VK_USE_PLATFORM_WIN32_KHR
    const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
#elif defined VK_USE_PLATFORM_MACOS_MVK
    const std::vector<const char*> deviceExtensions = {
        "VK_KHR_portability_subset",
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
#else
    const std::vector<const char*> deviceExtensions = {
   VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
#endif
    
private:
    VkInstance _instance;
    VkDevice _device;
    VkSurfaceKHR _surface;
    VkSwapchainKHR _swapChain;

    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    std::vector<VkImageView> _swapChainImageViews;
    
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;
    
    VkRenderPass _renderPass;
    VkPipelineLayout _pipelineLayout;
    
    VkPipeline _graphicsPipeline;
    VkCommandPool _commandPool;
    VkCommandBuffer _commandBuffer;
    
    // スワップチェーンから画像が取得できたのでレンダリングができているということを示すためのセマフォ
    VkSemaphore _imageAvailableSemaphore;
    // レンダリングが終了してプレゼンテーションが行われる可能性があることを示すためのセマフォ
    VkSemaphore _renderFinishedSemaphore;
    // 一度に一つのフレームのみがレンダリングされることを示すフェンス
    VkFence _inFlightFence;

    
    std::vector<VkFramebuffer> _swapChainFramebuffers;
    
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    
};

#endif /* Renderer_hpp */
