#include "vulkan_performance_settings.h"

namespace pbr::shared::apis::graphics {
    VkSampleCountFlagBits get_msaa_samples(const performance_settings& settings,
                                           VkSampleCountFlagBits max_samples) {
        VkSampleCountFlagBits samples {VK_SAMPLE_COUNT_1_BIT};

        if (settings.msaa_samples >= 0 && settings.msaa_samples <= 1) {
            samples = VK_SAMPLE_COUNT_1_BIT;
        } else if (settings.msaa_samples <= 2) {
            samples = VK_SAMPLE_COUNT_2_BIT;
        } else if (settings.msaa_samples <= 4) {
            samples = VK_SAMPLE_COUNT_4_BIT;
        } else if (settings.msaa_samples <= 8) {
            samples = VK_SAMPLE_COUNT_8_BIT;
        } else if (settings.msaa_samples <= 16) {
            samples = VK_SAMPLE_COUNT_16_BIT;
        } else if (settings.msaa_samples <= 32) {
            samples = VK_SAMPLE_COUNT_32_BIT;
        } else if (settings.msaa_samples <= 64) {
            samples = VK_SAMPLE_COUNT_64_BIT;
        }

        if (samples > max_samples) {
            samples = max_samples;
        }

        return samples;
    }
}
