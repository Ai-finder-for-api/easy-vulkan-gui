#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace EasyVK {

class Instance {
public:
    Instance(const std::string& appName, bool enableValidation);
    ~Instance();
    VkInstance get() const;
private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    bool validationEnabled = false;
    void setupDebugMessenger();
};

}