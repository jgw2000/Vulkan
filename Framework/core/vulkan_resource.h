#pragma once

namespace vkb::core
{
    class HPPDevice;

    /// Inherit this for any Vulkan object with a handle of type `Handle`.
    ///
    /// This allows the derived class to store a Vulkan handle, and also a pointer to the parent vkb::Device.
    /// It also allows to set a debug name for any Vulkan object.
    template <typename Handle>
    class VulkanResource
    {
    public:
        VulkanResource(Handle handle = nullptr, HPPDevice* device_ = nullptr);
        virtual ~VulkanResource() = default;

        VulkanResource(const VulkanResource&) = delete;
        VulkanResource& operator=(const VulkanResource&) = delete;

        VulkanResource(VulkanResource&& other);
        VulkanResource& operator=(VulkanResource&& other);

        HPPDevice&         get_device()           { return *device; }
        const HPPDevice&   get_device() const     { return *device; }
        Handle&            get_handle()           { return handle; }
        const Handle&      get_handle() const     { return handle; }
        bool               has_device() const     { return device != nullptr; }
        bool               has_handle() const     { return handle != nullptr; }
        void               set_handle(Handle hdl) { handle = hdl; }

    private:
        HPPDevice*  device;
        Handle      handle;
    };
}
