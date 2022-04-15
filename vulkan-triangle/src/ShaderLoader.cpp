//
//  ShaderLoader.cpp
//  command20220406
//
//  Created by 株式会社アフェクション on 2022/04/11.
//

#include "ShaderLoader.hpp"


std::vector<char> ShaderLoader::ReadFile(const std::string& filename){
    
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if(!file.is_open()){
        throw std::runtime_error("failed to open file");
    }
    
    // 読み取り位置を最後に指定してファイルサイズを決定する。
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    
    // ファイルの先頭に戻り、
    // 全てのバイトを一度によみとることができる。
    file.seekg(0);
    file.read(buffer.data(),fileSize);
    
    file.close();
    
    return buffer;
}

VkShaderModule ShaderLoader::CreateShaderModule(VkDevice& device,const std::vector<char>& code){
    
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    
    return shaderModule;
};
