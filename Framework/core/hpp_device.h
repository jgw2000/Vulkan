#pragma once

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

        bool is_extension_supported(const std::string& extension) const;
        
        bool is_enabled(const std::string& extension) const;

        uint32_t get_queue_family_index(vk::QueueFlagBits queue_flag) const;

    private:
        const HPPPhysicalDevice& gpu;

        vk::SurfaceKHR surface{ nullptr };

        std::vector<const char*> enabled_extensions{};

        std::vector<std::vector<HPPQueue>> queues;
    };
}
