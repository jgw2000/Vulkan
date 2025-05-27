#pragma once

#include "hpp_resource_cache.h"

namespace vkb::core
{
    class HPPPhysicalDevice;

    class HPPDevice : public VulkanResource<vk::Device>
    {
    public:
        /**
         * @brief HPPDevice constructor
         * @param gpu A valid Vulkan physical device and the requested gpu features
         * @param surface The surface
         * @param requested_extensions (Optional) List of required device extensions
         */
        HPPDevice(HPPPhysicalDevice&        gpu,
                  vk::SurfaceKHR            surface,
                  std::vector<const char*>  requested_extensions = {});
        ~HPPDevice();

        HPPDevice(const HPPDevice&) = delete;
        HPPDevice(HPPDevice&&) = delete;
        HPPDevice& operator=(const HPPDevice&) = delete;
        HPPDevice& operator=(HPPDevice&&) = delete;

        const HPPPhysicalDevice& get_gpu() const { return gpu; }

        const HPPQueue& get_queue(uint32_t queue_family_index, uint32_t queue_index) const;

        const HPPQueue& get_queue_by_flags(vk::QueueFlags queue_flags, uint32_t queue_index) const;

        const HPPQueue& get_queue_by_present(uint32_t queue_index) const;

        /**
         * @brief Finds a suitable graphics queue to submit to
         * @return The first present supported queue, otherwise just any graphics queue
         */
        const HPPQueue& get_suitable_graphics_queue() const;

        bool is_extension_supported(const std::string& extension) const;
        
        bool is_enabled(const std::string& extension) const;

        uint32_t get_queue_family_index(vk::QueueFlagBits queue_flag) const;

        vkb::HPPResourceCache& get_resource_cache() { return resource_cache; }

    private:
        const HPPPhysicalDevice& gpu;

        vk::SurfaceKHR surface{ nullptr };

        std::vector<const char*> enabled_extensions{};

        std::vector<std::vector<HPPQueue>> queues;

        // A command pool associated to the primary queue
        // std::unique_ptr<HPPCommandPool> command_pool;

        vkb::HPPResourceCache resource_cache;
    };
}
