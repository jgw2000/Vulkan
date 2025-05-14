#pragma once

#include "hpp_instance.h"

namespace vkb::core
{
    class HPPInstance;

    struct DriverVersion
    {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
    };

    /**
     * @brief A wrapper class for vk::PhysicalDevice
     *
     * This class is responsible for handling gpu features, properties, and queue families for the device creation.
     */
    class HPPPhysicalDevice
    {
    public:
        HPPPhysicalDevice(HPPInstance& instance, vk::PhysicalDevice physical_device);

        HPPPhysicalDevice(const HPPPhysicalDevice&) = delete;
        HPPPhysicalDevice(HPPPhysicalDevice&&) = delete;
        HPPPhysicalDevice& operator=(const HPPPhysicalDevice&) = delete;
        HPPPhysicalDevice& operator=(HPPPhysicalDevice&&) = delete;

        /**
         * @brief Used at logical device creation to pass the extensions feature chain to vkCreateDevice
         * @returns A void pointer to the start of the extension linked list
         */
        void* get_extension_feature_chain() const;

        bool is_extension_supported(const std::string& requested_extension) const;

        const vk::PhysicalDeviceFeatures& get_features() const;

        vk::PhysicalDevice get_handle() const;

        HPPInstance& get_instance() const;

        const vk::PhysicalDeviceMemoryProperties& get_memory_properties() const;

        const vk::PhysicalDeviceProperties& get_properties() const;

        const std::vector<vk::QueueFamilyProperties>& get_queue_family_properties() const;

        const vk::PhysicalDeviceFeatures get_requested_features() const;

        vk::PhysicalDeviceFeatures& get_mutable_requested_features();

        /**
         * @brief Sets whether or not the first graphics queue should have higher priority than other queues.
         * Very specific feature which is used by async compute samples.
         * @param enable If true, present queue will have prio 1.0 and other queues have prio 0.5.
         * Default state is false, where all queues have 0.5 priority.
         */
        void set_high_priority_graphics_queue_enable(bool enable)
        {
            high_priority_graphics_queue = enable;
        }

        /**
         * @brief Returns high priority graphics queue state.
         * @return High priority state.
         */
        bool has_high_priority_graphics_queue() const
        {
            return high_priority_graphics_queue;
        }

    private:
        // Handle to the Vulkan instance
        HPPInstance& instance;

        // Handle to the Vulkan physical device
        vk::PhysicalDevice handle{nullptr};

        // The extensions that this GPU supports
        std::vector<vk::ExtensionProperties> device_extensions;

        // The GPU properties
        vk::PhysicalDeviceProperties properties;

        // The GPU memory properties
        vk::PhysicalDeviceMemoryProperties memory_properties;

        // The features that this GPU supports
        vk::PhysicalDeviceFeatures features;

        // The features that will be requested to be enabled in the logical device
        vk::PhysicalDeviceFeatures requested_features;

        // The extension feature pointer
        void* last_requested_extension_feature{ nullptr };

        // The GPU queue family properties
        std::vector<vk::QueueFamilyProperties> queue_family_properties;

        // Holds the extension feature structures, we use a map to retain an order of requested structures
        std::map<vk::StructureType, std::shared_ptr<void>> extension_features;

        bool high_priority_graphics_queue{ false };
    };
}
