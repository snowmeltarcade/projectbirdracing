#include "application_window.h"

#include <SDL_vulkan.h>

namespace pbr::shared::apis::windowing {
    bool application_window::create(std::string_view title,
                                    uint32_t x, uint32_t y,
                                    uint32_t w, uint32_t h) noexcept {
        auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow(std::string(title).c_str(),
                                         x, y,
                                         w, h,
                                         flags);

        if (!this->_window) {
            this->_log_manager->log_message("Failed to create SDL window.", apis::logging::log_levels::error);
            return false;
        }

        VkApplicationInfo ai;
        memset(&ai, 0, sizeof(ai));
        ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ai.pApplicationName = "Project Bird Racing";
        ai.applicationVersion = VK_MAKE_API_VERSION(0, 0, 4, 0);
        ai.pEngineName = "PBR";
        ai.engineVersion = VK_MAKE_API_VERSION(0, 0, 4, 0);
        ai.apiVersion = VK_API_VERSION_1_2;

        auto extensions_count {0u};
        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, nullptr);

        std::vector<const char*> extensions;
        extensions.resize(extensions_count);

        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, extensions.data());

        VkInstanceCreateInfo ii;
        memset(&ii, 0, sizeof(ii));
        ii.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        ii.pApplicationInfo = &ai;
        ii.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        ii.ppEnabledExtensionNames = extensions.data();
        ii.enabledLayerCount = 0u;

        if (vkCreateInstance(&ii, nullptr, &this->_vulkan_instance) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create Vulkan instance.", apis::logging::log_levels::error);
            return false;
        }

        return true;
    }

    void application_window::shutdown() noexcept {
        if (this->_vulkan_instance) {
            vkDestroyInstance(this->_vulkan_instance, nullptr);
            this->_vulkan_instance = nullptr;
        }

        if (this->_window) {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }
}
