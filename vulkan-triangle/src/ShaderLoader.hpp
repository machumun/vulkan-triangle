//
//  ShaderLoader.hpp
//  command20220406
//
//  Created by 株式会社アフェクション on 2022/04/11.
//

#ifndef ShaderLoader_hpp
#define ShaderLoader_hpp

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

class ShaderLoader{
public:
    static std::vector<char> ReadFile(const std::string& filename);
    VkShaderModule CreateShaderModule(VkDevice&,const std::vector<char>& code);
    
};

#endif /* ShaderLoader_hpp */
