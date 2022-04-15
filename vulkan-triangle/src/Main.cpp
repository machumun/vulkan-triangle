#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


// for cause of error
#include <iostream>

// for exit_success macro
#include <stdexcept>

#include "Renderer.hpp"

#define PLATFORM_WINDOWS

const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;



class HelloTriangleApplication{
    
public:
    void Run(){
        _InitWindow();
        _InitVulkan();
        _MainLoop();
        _Cleanup();
    }
    GLFWwindow* window = 0;
    Renderer renderer;
private:
    
    int _InitWindow(){
        
        if (glfwInit() == GL_FALSE){
                // 初期化に失敗した
                std::cerr << "Can't initialize GLFW" << std::endl;
            return -1;
        }
            // ウィンドウを作成する
           
        
        glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        window = (glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "vulkan 300", NULL, NULL));
        
        if(window == NULL){
            // ウィンドウが作成できなかった
            std::cerr << "Can't create GLFW window." << std::endl;
            glfwTerminate();
            return -1;
        }
        
        return EXIT_SUCCESS;
    }
    
    // init
    void _InitVulkan(){
        renderer.Create(window);
    }
    
    // loop
    void _MainLoop(){
        while(glfwWindowShouldClose(window) == VK_FALSE){
            glfwPollEvents();
            renderer.DrawFrame();
        }
        
        renderer.DeviceWaitIdle();
    }
    
    // cleaner
    void _Cleanup(){
        renderer.Destroy();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
};

int main()
{
    HelloTriangleApplication app;
    
    try{
        app.Run();
    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
