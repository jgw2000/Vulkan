#pragma once

namespace vkb::core
{
    class HPPDevice;
}

namespace vkb::allocated
{
    /**
     * @brief Retrieves a reference to the VMA allocator singleton.  It will hold an opaque handle to the VMA
     * allocator between calls to `init` and `shutdown`.  Otherwise it contains a null pointer.
     * @return A reference to the VMA allocator singleton handle.
     */
    VmaAllocator& get_memory_allocator();

    /**
     * @brief The non-templatized VMA initializer function, referenced by the template version to smooth
     * over the differences between the `vkb::Device` and `vkb::core::HPPDevice` classes.
     * Idempotent, but should be paired with `shutdown`.
     * @param create_info The VMA allocator create info.
     */
    void init(const VmaAllocatorCreateInfo& create_info);

    /**
     * @brief Initializes the VMA allocator with the specified device, expressed
     * as the `vkb` wrapper class, which might be `vkb::Device` or `vkb::core::HPPDevice`.
     * @tparam DeviceType The type of the device.
     * @param device The Vulkan device.
     */
    void init(const vkb::core::HPPDevice& device);

    /**
     * @brief Shuts down the VMA allocator and releases all resources.  Should be preceeded with a call to `init`.
     */
    void shutdown();
}