//
//  Renderer.cpp
//  command20220406
//
//  Created by Hidesato on 2022/04/07.
//

#include "Renderer.hpp"


// キホンVulkanにはカスタムアロケータに関するあロケーターコールバックがあり、nullptrを渡すと無視できる。
// 使いまわせるようにだしただけ
void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // messageの重要度
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    // type
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // コールバック関数へのポインタ
    createInfo.pfnUserCallback = userValidation.DebugCallback;
    
    // クラスへのポインタを渡すことができる この場合はHelloTriangleApplicationか？
    createInfo.pUserData = nullptr; // Optional
}

void Renderer::SetupDebugMessenger(){
    if(userValidation.enableValidationLayers){
        return;
    }
    // メッセンジャーとコールバックに関する構造体に情報を入力
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);
    
    // nullptr部分はアロケータ
    // デバッグ検証レイヤーを作成する。
    if(userValidation.CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger)!=VK_SUCCESS){
        throw std::runtime_error("failed to set up debug messenger!");
    }
    
}

void Renderer::DestoroyInstance(){
    
    if(userValidation.enableValidationLayers){
        userValidation.DestroyDebugUtilsMessengerEXT(_instance, nullptr, _debugMessenger);
    }
    
    vkDestroyInstance(_instance, nullptr);
    
}

void Renderer::DestroyDevice(){
    vkDestroyDevice(_device, nullptr);
}

void Renderer::DestroySurface() {
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
}

void Renderer::CreateInstance(){
    
   
    userValidation.ValidationCheck();
    
    // アプリケーションの情報
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    // グローバル(<-プログラム全体)拡張機能と検証レイヤーをVulkanドライバに指示するもの
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // グローバル拡張機能を指定する。GLFWにある便利な組み込み関数。
//    uint32_t glfwExtensionCount = 0;
//    const char** glfwExtensions = nullptr;
//    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> requiredExtensionNames = userValidation.GetRequiredExtensions();
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensionNames.size());
    createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();
    
    // 有効にするグローバル検証レイヤー情報。ここで検証レイヤーつかう
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(userValidation.enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(userValidation.validationLayers.size());
        createInfo.ppEnabledLayerNames = userValidation.validationLayers.data();
        
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        // pNextがなんなのかわからんが、デバッグ用のcreateInfo参照をpPextにいれておく.
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }else{
        createInfo.enabledLayerCount = 0;
        
        createInfo.pNext = nullptr;
    }
    
    
    // 引数は (作成情報の構造体ポインター,カスタムアロケータへのポインタ、ハンドルを格納するinstance変数のポインタ)
    VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
    
    if(result != VK_SUCCESS){
        throw std::runtime_error("failed to create");
    }
    
    // サポートされている拡張機能のリストを取得する
    uint32_t extensionCount = 0;
    // nullptrを引数にいれておくことで、拡張機能の数だけをもらえる(?)
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    
    //拡張機能の詳細を保持する動的配列
    std::vector<VkExtensionProperties> extensions(extensionCount);
    
    // 拡張機能の情報が入っている動的配列の先頭のポインタを渡して、詳細を照会できる
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    
    std::cout << "available extensions:¥n";
    
    
    // VkExtensionProperties構造体には拡張機能の名前とバージョンが含まれている。
    // 単純なforループで確認。
    for(const auto& extension : extensions){
        std::cout << '\t' << extension.extensionName << '\n';
    }
    // glfwGetRequiredInstanceExtensionsで必要な拡張機能を調べることができる。
    // 拡張機能の要件をチェックする関数をつくらなければ。。。
    
}

// デバイスの適合性チェック
bool Renderer::IsDeviceSuitable(VkPhysicalDevice device) {

    QueueFamilyIndices indices = FindQueueFamilies(device);
    // 基本的なプロパティの照会
//    VkPhysicalDeviceProperties deviceProperties;
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    // テクスチャ圧縮やマルチビューポートレンダリングのサポートの照会
//    VkPhysicalDeviceFeatures deviceFeatures;
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    // 例えば、ジオメトリシェーダーをサポートする専用のグラフィックカードのみ使用できような設定。
//    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    
    // 今回はVulkanサポートだけがあればいい。つまりGPUさえあればいい(?)
//    return true;
    
    // というわけにもいかず、必要なキューファミリーがあるデバイスであるかどうかを判別するということにする。

    // swapchainサポートも確認しておく
    bool extensionSupported = CheckDeviceExtensionSupport(device);
    bool swapChainAdequate = false;

    if (extensionSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }


    return indices.isComplete() && extensionSupported && swapChainAdequate;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device) {


    // 利用可能なデバイス拡張機能を取得
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // 必要なデバイス拡張のリストをset
    // std::vectorのイテレータを放り込む。
    // 何をやっているのか
    std::set< std::string > requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

// オリジナルの物理デバイス評価形態。これを使うのはまたの機会か
//int rateDeviceSuitablity(VkPhysicalDevice device){
//    int score = 0;
//
//        // Discrete GPUs have a significant performance advantage
//        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
//            score += 1000;
//        }
//
//        // Maximum possible size of textures affects graphics quality
//        score += deviceProperties.limits.maxImageDimension2D;
//
//        // Application can't function without geometry shaders
//        if (!deviceFeatures.geometryShader) {
//            return 0;
//        }
//
//    return score;
//}

void Renderer::PickPhysicalDevice(){
    uint32_t deviceCount = 0;
    // 拡張機能の一覧表示と似ており、番号だけを紹介することからはじまる。
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    
    if(deviceCount == 0){
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
    
    for(const auto& device : devices){
        if(IsDeviceSuitable(device)){
            _physicalDevice = device;

        }
    }
    
    if(_physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    // 別のプロセスで評価してもいい。
}

Renderer::QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;
    
    // まさにあなたが期待するプロセスでキュファミリの参照一覧をいただく。
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int i=0;
    //VK_QUEUE_GRAPHICS_BITをサポートするキューファミリーを少なくとも一つ見つける。
    for(const auto& queueFamily : queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if(indices.isComplete()){
            break;
        }
        
        i++;
    }
    
    return indices;
}

void Renderer::CreateLogicalDevice(){
    QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),indices.presentFamily.value() };
    
    // 論理デバイスの作成はインスタンス作成に似ている。
    // グラフィック機能を備えたキューにのみ関心がある。
    // キューファミリーという名前であるが、ファミリーほどのキューは作成できない。低オーバーヘッドを実現するため？
     //キューの優先順位。キューが一つしかない場合も設定しておかなければいけない。
    float queuePriority = 1.0f;

    for(uint32_t queueFamily:uniqueQueueFamilies){
    
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    
   
    
   
    
    // 使用するデバイス機能のセット。Vulkanで面白いことをやろうとしたらここを凝って機能をピックアップしてあげる。
    // 今回はなんもいらんから初期化したやつをそのままぶちこむ。
    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // 上でつくったもの

    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    
    createInfo.pEnabledFeatures = &deviceFeatures;
    

    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
   
    
    //swapchainでここにもどってくる。
    if (userValidation.enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(userValidation.validationLayers.size());
        createInfo.ppEnabledLayerNames = userValidation.validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    
    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    
    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
}

void Renderer::CreateSurface(GLFWwindow* window) {

    if (glfwCreateWindowSurface(_instance, window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}


// スワップチェーン自作構造体に中身を詰める用の関数
Renderer::SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice device) {

    SwapChainSupportDetails details;

    // 第三引数はVkSurfaceCapabilitiesKHR
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);


    // サポートされているサーフェスの形式をいただく
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
    if (formatCount != 0) {

        // vector::resizeはセットアップでは使ってもいいのか?
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
    }

    // サポートされているプレゼンテンションモード
    // プレゼンテーションモードとは・・・スワップチェーンにとって重要な設定
    // VK_PRESENT_MODE_FIFO_KHR
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        // vectorのサイズ変更
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
    }

    // 詳細を構造体に代入終わり


    return details;

}

// 
VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    // 失敗した場合は最初のフォーマットを返す。

    return availableFormats[0];

}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

    // VK_PRESENT_MODE_FIFO_KHRは最低限保証されている
    // VK_PRESENT_MODE_MAILBOX_KHRはエネルギー使用量にレイテンシを低くしながらティアリングを回避
    // モバイルデバイスではVK_PRESENT_MODE_FIFO_KHRを使用することをおすすめ

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;

}


// スワップ範囲？
// デバイスと表示ウィンドウの解像度の変換をとりもつ?
// 許可された範囲のclamp
VkExtent2D Renderer::ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {

    if(capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()){
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window,&width,&height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
    

};


// 上三つとりまとめ+α
void Renderer::CreateSwapChain(GLFWwindow* window){

    // 自作構造体
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice);


    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(window,swapChainSupport.capabilities);

    // VkSurfaceCapabilitiesKHRのmnImageCount
    // スワップチェーンに含めるイメージの数。ドライバーの内部操作完了の関係で一つ多めにすることが
    // 推奨されている
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount+1;

   

    // maxImageCountが0のとき、最大値がないことを表す。
    // よってmaxImageCountに最大値が存在し、かつ最大値を超えてしまったときは
    // 最大値に収束させる
    if(swapChainSupport.capabilities.maxImageCount>0 && imageCount>swapChainSupport.capabilities.maxImageCount){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // 伝統芸createInfo
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    // swapchainは関連付ける必要がある。
    createInfo.surface = _surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;

    // 3dアプリはレイヤー量1
    createInfo.imageArrayLayers = 1;
    // スワップチェーン内の画像を使用する操作の種類
    // 直接レンダリングをするので、カラーアタッチメントとして使用.?
    // postprocessなどをかける場合はvk_imageUsage代わりのような値を使用して
    // メモリ操作を使用する
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    // イメージが一度に一つのキューファミリによって所有される。
    // パフォ－マンス最高
     //std::cout << indices.graphicsFamily.value() << std::endl;
    //std::cout << indices.presentFamily.value() << std::endl;
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    // 複数のキューファミリ間でイメージを使用。
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    // 回転や方向変換、アルファブレンドの方法
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;

    // 画素塗りつぶしの設定？最高パフォーマンスを得るためにVK_TRUEにしておく。
    createInfo.clipped = VK_TRUE;

    // アプリ実行中にスワップチェーンが無効になったり、最適化されない可能性がある。
    // windowサイズが変更されたりなど。
    // この場合、スワップチェーンを最初から再作成しないといけないが、
    // その前の古いスワップチェーンの参照をここに指定しておく必要がある。
    // 今のところ、リサイズに非対応ということにしておく。
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(_device,&createInfo,nullptr,&_swapChain)!=VK_SUCCESS){
        throw std::runtime_error("failed to create swap chain!");
    }

    // スワップチェーンイメージの取得
   
    // イメージの最小数を指定したため、
//    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
    
    
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
//    std::cout << _swapChainImages.size() << std::endl;
    // 後から使うのでフォーマットとextentをメンバ変数の保存。
    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;

}

void Renderer::DestroySwapChain() {
    vkDestroySwapchainKHR(_device,_swapChain,nullptr);
}

void Renderer::CreateImageViews() {

    _swapChainImageViews.resize(_swapChainImages.size());

    // すべてのスワップチェーンイメージを処理するループ
    for (size_t i = 0; i < _swapChainImages.size(); ++i) {
        
        
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.image = _swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = _swapChainImageFormat;
        // swizzleとは...
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

//        std::cout << i << std::endl;

        if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }

}

void Renderer::DestroyImageViews() {

    for (auto imageView : _swapChainImageViews) {
        vkDestroyImageView(_device, imageView, nullptr);
    }
   
}

void Renderer::CreateRenderPass(){
    
    // 今回はカラーバッファアタッチメントが一つだけあるのみ
    VkAttachmentDescription colorAttachment{};
    // スワップチェーンの画像の形式と同じを放り込む
    colorAttachment.format = _swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // 開始時にクリア操作でフレームバッファを黒に塗りつぶす。
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // レンダリングされたコンテンツはメモリに保存しておいてあとで読み取ることができる
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    
    // loadOP,storeOpは色と深度のデータに適用される。
    // stencilLoadOp,stencilStoreOpはステンシルバッファに適用されるが、現在は扱わないのでDONT_CARE
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    // レンダリングパスのイニシャライズ。今回は黒で塗りつぶすのでUNDEFINEDでよい
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // スワップチェーンで表示される画像として設定しておく.
    // カラーアタッチメントとして使用される画像や、メモリコピーの宛先として使用される画像にもできる。
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    // サブレンダーパス。前のパスのフレームバッファの内容に依存する後続のレンダリング操作.
    VkAttachmentReference colorAttachmentRef{};
    // 参照する添付ファイル(シェーダファイル？)のインデックス。単一のVkAttachmentReferenceのため0
    colorAttachmentRef.attachment = 0;
    // アタッチメントをカラーバッファーとして使用する予定であるので、以下。
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    // フラグメントシェーダファイル内のlayout(location = 0) out vec4 outColorが参照として格納される。
    subpass.pColorAttachments = &colorAttachmentRef;
    
    //サブパスの依存関係
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    
    
}

void Renderer::DestroyRenderPass(){
    vkDestroyRenderPass(_device, _renderPass, nullptr);
}

void Renderer::CreateGraphicsPipeline(){

    auto vertShaderCode = shaderLoader.ReadFile("vert.spv");
    auto fragShaderCode = shaderLoader.ReadFile("frag.spv");

    VkShaderModule vertShaderModule = shaderLoader.CreateShaderModule(_device,vertShaderCode);
    VkShaderModule fragShaderModule = shaderLoader.CreateShaderModule(_device,fragShaderCode);
    
    // パイプライン作成プロセスの一部として構造を介して特定のパイプラインステージにシェーダ
    // を割り当てる必要がある。
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    
    // フラグメントシェーダ
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // バインディング・・データ間の感覚とデータが頂点毎かインスタンス毎か
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    // 属性の説明：頂点シェーダーに渡される
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
    
    // 入力アセンブリ.頂点の読み込み形式(バッファ再利用するかしないかなど・・)
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // ビューポート
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) _swapChainExtent.width;
    viewport.height = (float) _swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    // 2dマスク.画面全体にしておく
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapChainExtent;
    
    // 上二つの情報をビューポート状態に結合しておく
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    //ラスタライザ
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // ポリゴンを塗りつぶすモード
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // その他の設定
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    
    // マルチサンプリング.今はとりあえず無効にしておく
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    
    // 深度とステンシルのテスト
    
    //カラーブレンド.二つの方法があり、
    // 古い値と新しい値を混ぜ合わせて最終的な色を作成 or bit演算
    // 今はとりあえずfalse
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    
    // 上とどっちかを選べば良い？
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
    
    // 上の構造体たちの状態はパイプラインを再作成せずに実際に変更ができる。
    // ビューポートサイズ、ブレンドの定数など。
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    
    //グラフィックスパイプライン作成情報の総まとめ
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
    
    // 第二引数については、あとの章
    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    
    vkDestroyShaderModule(_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(_device, vertShaderModule, nullptr);
};

void Renderer::DestoyGraphicsPipeline(){
    vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
}

void Renderer::CreateFrameBuffers(){
    // フレームバッファを保持するようにコンテナのサイズを変更(初期化?)する
    _swapChainFramebuffers.resize(_swapChainImageViews.size());
    
    // イメージビューを繰り返して編集してフレームバッファを作成する。
    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            _swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _swapChainExtent.width;
        framebufferInfo.height = _swapChainExtent.height;
        framebufferInfo.layers = 1;
        
        // フレームバッファは互換性のあるレンダーパスでのみ使用ができる。
        if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Renderer::DestroyFrameBuffers(){
    for (auto framebuffer : _swapChainFramebuffers) {
           vkDestroyFramebuffer(_device, framebuffer, nullptr);
    }
}

void Renderer::CreateCommandPool(){
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(_physicalDevice);
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // フレーム毎にコマンドバッファをリセットして個別に再記録するためのフラグ。
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void Renderer::DestroyCommandPool(){
    vkDestroyCommandPool(_device, _commandPool, nullptr);
}

void Renderer::CreateCommandBuffer(){
    //先のコマンドプールと割り当てるバッファの数を指定するパラメータを構造体にいれてつくる。
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(_device, &allocInfo, &_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void Renderer::DestroyCommandBuffer(){
//    vkDestroyCo
}

//実行するコマンドをコマンドバッファに書き込む関数
void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    
    // レンダリングパスの開始
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass;
    renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapChainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // グラフィックスパイプラインか計算パイプラインかを示す。レンダリングパスを終了できる。
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Renderer::DrawFrame(){
    // 前のフレームが修了するまで待機をしてコマンドバッファとセマフォを使用できるようにする
    // UINT64_MAXはタイムアウトが事実上無効となる。
    vkWaitForFences(_device, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_device, 1, &_inFlightFence);
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    
    vkResetCommandBuffer(_commandBuffer, 0);
    
    RecordCommandBuffer(_commandBuffer, imageIndex);
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;
    
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    vkQueuePresentKHR(_presentQueue, &presentInfo);
}

void Renderer::CreateSyncObjects(){
    // セマフォ作成
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // フェンスの作成
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // 初回フレームはフェンスによる通知がこない。
    // そのあたりの問題を回避するため、flagを設定しておく
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS){
        throw std::runtime_error("failed to create semaphores!");
    }
    
}

void Renderer::DestroySyncObjects(){
    vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
    vkDestroyFence(_device, _inFlightFence, nullptr);
}

void Renderer::Create(GLFWwindow* window){
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface(window);
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapChain(window);
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFrameBuffers();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
}

void Renderer::Destroy(){
    DestroySyncObjects();
    DestroyCommandBuffer();
    DestroyCommandPool();
    DestroyFrameBuffers();
    DestoyGraphicsPipeline();
    DestroyRenderPass();
    DestroyImageViews();
    DestroySwapChain();
    DestroySurface();
    DestroyDevice();
    DestoroyInstance();
}
