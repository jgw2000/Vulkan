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
         * @brief Get an extension features struct
         *
         *        Gets the actual extension features struct with the supported flags set.
         *        The flags you're interested in can be set in a corresponding struct in the structure chain
         *        by calling PhysicalDevice::add_extension_features()
         * @returns The extension feature struct
         */
        template <typename HPPStructureType>
        HPPStructureType get_extension_features()
        {
            // We cannot request extension features if the physical device properties 2 instance extension isn't enabled
            if (!instance.is_enabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            {
                throw std::runtime_error("Couldn't request feature from device as " +
                      std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) + " isn't enabled!");
            }

            // Get the extension feature
            return handle.getFeatures2KHR<vk::PhysicalDeviceFeatures2KHR, HPPStructureType>().template get<HPPStructureType>();
        }

        /**
         * @brief Add an extension features struct to the structure chain used for device creation
         *
         *        To have the features enabled, this function must be called before the logical device
         *        is created. To do this request sample specific features inside
         *        VulkanSample::request_gpu_features(vkb::HPPPhysicalDevice &gpu).
         *
         *        If the feature extension requires you to ask for certain features to be enabled, you can
         *        modify the struct returned by this function, it will propagate the changes to the logical
         *        device.
         * @returns A reference to the extension feature struct in the structure chain
         */
        template <typename HPPStructureType>
        HPPStructureType& add_extension_features()
        {
            // We cannot request extension features if the physical device properties 2 instance extension isn't enabled
            if (!instance.is_enabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            {
                throw std::runtime_error("Couldn't request feature from device as " +
                      std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) + " isn't enabled!");
            }

            // Add an (empty) extension features into the map of extension features
            auto [it, added] = extension_features.insert({ HPPStructureType::structureType, std::make_shared<HPPStructureType>() });
            if (added)
            {
                // if it was actually added, also add it to the structure chain
                if (last_requested_extension_feature)
                {
                    static_cast<HPPStructureType*>(it->second.get())->pNext = last_requested_extension_feature;
                }
                last_requested_extension_feature = it->second.get();
            }

            return *static_cast<HPPStructureType*>(it->second.get());
        }

        /**
         * @brief Request an optional features flag
         *
         *        Calls get_extension_features to get the support of the requested flag. If it's supported,
         *        add_extension_features is called, otherwise a log message is generated.
         *
         * @returns true if the requested feature is supported, otherwise false
         */
        template <typename Feature>
        vk::Bool32 request_optional_feature(vk::Bool32 Feature::* flag, std::string const& featureName, std::string const& flagName)
        {
            vk::Bool32 supported = get_extension_features<Feature>().*flag;
            if (supported)
            {
                add_extension_features<Feature>().*flag = true;
            }

            return supported;
        }

        /**
         * @brief Request a required features flag
         *
         *        Calls get_extension_features to get the support of the requested flag. If it's supported,
         *        add_extension_features is called, otherwise a runtime_error is thrown.
         */
        template <typename Feature>
        void request_required_feature(vk::Bool32 Feature::* flag, const std::string& featureName, const std::string& flagName)
        {
            if (get_extension_features<Feature>().*flag)
            {
                add_extension_features<Feature>().*flag = true;
            }
            else
            {
                throw std::runtime_error(std::string("Requested required feature <") + featureName + "::" + flagName + "> is not supported");
            }
        }

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
