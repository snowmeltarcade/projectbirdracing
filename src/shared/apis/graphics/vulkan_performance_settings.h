/// Separate out any Vulkan specific conversions needed to use the `performance_settings` struct

#pragma once

#include "performance_settings.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics {
    /// Returns the MSAA samples in Vulkan units
    /// \param settings The performance settings
    /// \returns The MSAA samples in Vulkan units
    VkSampleCountFlagBits get_msaa_samples(const performance_settings& settings);
}
