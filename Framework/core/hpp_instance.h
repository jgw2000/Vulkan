#pragma once

namespace vkb::core
{
    class HPPPhysicalDevice;

    /**
     * @brief A wrapper class for vk::Instance
     *
     * This class is responsible for initializing the dispatcher, enumerating over all available extensions and validation layers
     * enabling them if they exist, setting up debug messaging and querying all the physical devices existing on the machine.
     */
    class HPPInstance
    {
    public:
        /**
         * @brief Can be set from the GPU selection plugin to explicitly select a GPU instead
         */
        static std::optional<uint32_t> selected_gpu_index;

        /**
         * @brief Can be set from the GPU selection plugin to explicitly select a GPU instead
         */

        /**
         * @brief Initializes the connection to Vulkan
         * @param application_name The name of the application
         * @param requested_extensions The extensions requested to be enabled
         * @param requested_layers The validation layers to be enabled
         * @param required_layer_settings The layer settings to be enabled
         * @param api_version The Vulkan API version that the instance will be using
         * @throws runtime_error if the required extensions and validation layers are not found
         */
        HPPInstance(const std::string&                      application_name,
                    const std::vector<const char*>&         requested_extensions    = {},
                    const std::vector<const char*>&         requested_layers        = {},
                    const std::vector<vk::LayerSettingEXT>& required_layer_settings = {},
                    uint32_t                                api_version             = VK_API_VERSION_1_0);

        /**
         * @brief Queries the GPUs of a vk::Instance that is already created
         * @param instance A valid vk::Instance
         */
        HPPInstance(vk::Instance instance);
        ~HPPInstance();

        HPPInstance(const HPPInstance&) = delete;
        HPPInstance(HPPInstance&&) = delete;
        HPPInstance& operator=(const HPPInstance&) = delete;
        HPPInstance& operator=(HPPInstance&&) = delete;

        const std::vector<const char*>& get_extensions();

        vk::Instance get_handle() const;

        /**
         * @brief Tries to find the first available discrete GPU that can render to the given surface
         * @param surface to test against
         * @returns A valid physical device
         */
        HPPPhysicalDevice& get_suitable_gpu(vk::SurfaceKHR surface);

        /**
         * @brief Checks if the given extension is enabled in the vk::Instance
         * @param extension An extension to check
         */
        bool is_enabled(const char* extension) const;

    private:
        /**
         * @brief Queries the instance for the physical devices on the machine
         */
        void query_gpus();

        /**
         * @brief The Vulkan instance
         */
        vk::Instance handle;

        /**
         * @brief The enabled extensions
         */
        std::vector<const char*> enabled_extensions;

#if defined(_DEBUG)
        /**
         * @brief Debug utils messenger callback for VK_EXT_Debug_Utils
         */
        vk::DebugUtilsMessengerEXT debug_utils_messenger;

        /**
         * @brief The debug report callback
         */
        vk::DebugReportCallbackEXT debug_report_callback;
#endif

        /**
         * @brief The physical devices found on the machine
         */
        std::vector<std::unique_ptr<HPPPhysicalDevice>> gpus;
    };
}
