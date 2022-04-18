#include "swap_chain.h"

#include <algorithm>
#include <array>

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Contains the supported swap chain details
    struct swap_chain_supported_details {
        /// The supported capabilities
        VkSurfaceCapabilitiesKHR capabilities;

        /// The available surface formats
        std::vector<VkSurfaceFormatKHR> surface_formats;

        /// The available present modes
        std::vector<VkPresentModeKHR> present_modes;
    };

    /// Returns what swap chain features etc... are supported by the passed physical device
    /// \param device The physical device
    /// \param surface The surface we will present to
    /// \returns The supported details
    swap_chain_supported_details query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface) noexcept {
        swap_chain_supported_details details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
                                                  surface,
                                                  &details.capabilities);

        auto format_count {0u};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                             surface,
                                             &format_count,
                                             nullptr);

        details.surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                             surface,
                                             &format_count,
                                             details.surface_formats.data());

        auto present_modes_count {0u};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                  surface,
                                                  &present_modes_count,
                                                  nullptr);

        details.present_modes.resize(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                  surface,
                                                  &present_modes_count,
                                                  details.present_modes.data());

        return details;
    }

    /// Select the surface format to use - prefer SRGB, else default to the first format in the passed formats
    /// \param formats The supported formats
    /// \returns The selected surface format
    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) noexcept {
        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }

        return formats[0];
    }

    /// Selects the present mode to use - prefer triple buffering, else default to FIFO
    /// \param modes The supported present modes
    /// \param prefer_vsync Should prefer using vsync?
    /// \returns The selected present mode
    VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& modes,
                                         bool prefer_vsync) noexcept {
        if (prefer_vsync) {
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        for (const auto& mode : modes) {
            // triple buffering
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
        }

        // this will always be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D select_swap_chain_extent(const VkSurfaceCapabilitiesKHR& capabilities,
                                        const std::shared_ptr<windowing::iapplication_window>& window) noexcept {
        // use the size Vulkan tells us to use
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            auto window_size = window->get_window_size();

            VkExtent2D extent {
                std::clamp(window_size.width_in_pixels,
                           capabilities.minImageExtent.width,
                           capabilities.maxImageExtent.width),

                std::clamp(window_size.height_in_pixels,
                           capabilities.minImageExtent.height,
                           capabilities.maxImageExtent.height),
            };

            return extent;
        }
    }

    /// Returns the number of images to use in this swap chain
    /// \param details The supported details
    /// \returns The number of images to use in this swap chain
    uint32_t get_number_of_images(const swap_chain_supported_details& details) noexcept {
        // try and use more than the minimum
        auto image_count = details.capabilities.minImageCount + 1;

        if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount) {
            image_count = details.capabilities.maxImageCount;
        }

        return image_count;
    }

    /// Creates the swap chain create info
    /// \param surface The display surface we will use
    /// \param image_count The number of images to use in the swap chain
    /// \param surface_format The surface format to use
    /// \param color_space The color format to use
    /// \param present_mode The present mode to use
    /// \param extent The size of the images in the swap chain
    /// \param old_swap_chain If we are re-creating the swap chain, the previous swap chain to use whilst
    /// \param queue_indexes The queue family indexes
    /// \param swap_chain_supported_details The supported swap chain details
    /// this new swap change is being created
    /// \returns The swap chain create info
    VkSwapchainCreateInfoKHR create_swap_chain_create_info(VkSurfaceKHR surface,
                                                           uint32_t image_count,
                                                           VkFormat surface_format,
                                                           VkColorSpaceKHR color_space,
                                                           VkPresentModeKHR present_mode,
                                                           VkExtent2D extent,
                                                           VkSwapchainKHR old_swap_chain,
                                                           physical_device::queue_family_indexes queue_indexes,
                                                           swap_chain_supported_details swap_chain_details) {
        VkSwapchainCreateInfoKHR create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = surface;
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format;
        create_info.imageColorSpace = color_space;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // if we are recreating the swap chain, let the old swap chain continue until the new one is created
        if (old_swap_chain != VK_NULL_HANDLE) {
            create_info.oldSwapchain = old_swap_chain;
        }

        auto are_graphics_queue_and_present_queue_the_same =
            *queue_indexes.graphics_family_index == *queue_indexes.present_family_index;

        if (are_graphics_queue_and_present_queue_the_same) {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;
            create_info.pQueueFamilyIndices = nullptr;
        } else {
            std::array<uint32_t, 2> indexes {
                static_cast<uint32_t>(*queue_indexes.graphics_family_index),
                static_cast<uint32_t>(*queue_indexes.present_family_index),
            };

            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = static_cast<uint32_t>(indexes.size());
            create_info.pQueueFamilyIndices = indexes.data();
        }

        // maybe useful for when a mobile device changes orientation?
        create_info.preTransform = swap_chain_details.capabilities.currentTransform;

        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        return create_info;
    }

    swap_chain::swap_chain(device& device,
                           physical_device& physical_device,
                           window_surface& surface,
                           performance_settings performance_settings,
                           std::shared_ptr<windowing::iapplication_window> application_window,
                           VkSwapchainKHR old_swap_chain_handle,
                           std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _log_manager(log_manager) {
        this->_log_manager->log_message("Creating swap chain...",
                                        logging::log_levels::info,
                                        "Vulkan");

        auto swap_chain_details = query_swap_chain_support(physical_device.get_native_handle(),
                                                           surface.get_native_handle());

        auto surface_format = choose_surface_format(swap_chain_details.surface_formats);

        auto present_mode = choose_present_mode(swap_chain_details.present_modes,
                                                performance_settings.prefer_vsync);

        auto extent = select_swap_chain_extent(swap_chain_details.capabilities,
                                               application_window);

        auto image_count = get_number_of_images(swap_chain_details);

        auto queue_indexes = physical_device.get_queue_family_indexes();

        auto create_info = create_swap_chain_create_info(surface.get_native_handle(),
                                                         image_count,
                                                         surface_format.format,
                                                         surface_format.colorSpace,
                                                         present_mode,
                                                         extent,
                                                         old_swap_chain_handle,
                                                         queue_indexes,
                                                         swap_chain_details);

        if (vkCreateSwapchainKHR(this->_device.get_native_handle(),
                                 &create_info,
                                 nullptr,
                                 &this->_swap_chain) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create swap chain.")
        }

        this->_image_format = create_info.imageFormat;
        this->_extent = create_info.imageExtent;

        if (!this->setup_images()) {
            FATAL_ERROR("Failed to setup images.")
        }

        this->_log_manager->log_message("Created swap chain.",
                                        logging::log_levels::info,
                                        "Vulkan");
    }

    swap_chain::~swap_chain() {
        if (this->_swap_chain) {
            vkDestroySwapchainKHR(this->_device.get_native_handle(), this->_swap_chain, nullptr);
            this->_swap_chain = nullptr;
        }
    }

    bool swap_chain::setup_images() noexcept {
        auto image_count {0u};
        vkGetSwapchainImagesKHR(this->_device.get_native_handle(),
                                this->_swap_chain,
                                &image_count,
                                nullptr);

        this->_images.resize(image_count);
        vkGetSwapchainImagesKHR(this->_device.get_native_handle(),
                                this->_swap_chain,
                                &image_count,
                                this->_images.data());

        if (!this->setup_image_views()) {
            this->_log_manager->log_message("Failed to setup image view.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        this->_log_manager->log_message("Created swap chain with `" + std::to_string(this->_images.size()) + "` images.",
                                        logging::log_levels::info,
                                        "Vulkan");

        return true;
    }

    bool swap_chain::setup_image_views() noexcept {
        // we don't want any destructors called when adding items to this list when memory is
        // moved, so reserve all we need up front
        this->_image_views.reserve(this->_images.size());

        for (const auto& image : this->_images) {
            this->_image_views.emplace_back(this->_device,
                                            image,
                                            this->_image_format,
                                            VK_IMAGE_ASPECT_COLOR_BIT,
                                            1,
                                            this->_log_manager);
        }

        return true;
    }
}
