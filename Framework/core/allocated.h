#pragma once

#include "core/vulkan_resource.h"

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

    /**
     * @brief The `Allocated` class serves as a base class for wrappers around Vulkan that require memory allocation
     * (`VkImage` and `VkBuffer`).  This class mostly ensures proper behavior for a RAII pattern, preventing double-release by
     * preventing copy assignment and copy construction in favor of move semantics, as well as preventing default construction
     * in favor of explicit construction with a pre-existing handle or a populated create info struct.
     *
     * This project uses the [VMA](https://gpuopen.com/vulkan-memory-allocator/) to handle the low
     * level details of memory allocation and management, as it hides away many of the messy details of
     * memory allocation when a user is first learning Vulkan, but still allows for fine grained control
     * when a user becomes more experienced and the situation calls for it.
     *
     * @note Constants used in this documentation in the form of `HOST_COHERENT` are shorthand for
     * `VK_MEMORY_PROPERTY_HOST_COHERENT_BIT` used for the sake of brevity.
     */
    template <typename HandleType>
    class Allocated : public vkb::core::VulkanResource<HandleType>
    {
    public:
        using DeviceType = vkb::core::HPPDevice;
        using ParentType = vkb::core::VulkanResource<HandleType>;

    public:
        Allocated()                 = delete;
        Allocated(const Allocated&) = delete;
        Allocated(Allocated&& other) noexcept;
        Allocated& operator=(const Allocated&) = delete;
        Allocated& operator=(Allocated&&)      = default;

    protected:
        /**
         * @brief The VMA-specific constructor for new objects. This should only be visible to derived classes.
         * @param allocation_create_info All of the non-resource-specific information needed by the VMA to allocate the memory.
         * @param args Additional constructor arguments needed for the derived class. Typically a `VkImageCreateInfo` or `VkBufferCreateInfo` struct.
         */
        template <typename... Args>
        Allocated(const VmaAllocationCreateInfo& allocation_create_info, Args&& ...args);

        /**
         * @brief This constructor is used when the handle is already created, and the user wants to wrap it in an `Allocated` object.
         * @note This constructor is used when the API provides us a pre-existing handle to something we didn't actually allocate, for instance
         * when we allocate a swapchain and access the images in it.  In these cases the `allocation` member variable will remain null for the
         * lifetime of the wrapper object (which is NOT necessarily the lifetime of the handle) and the wrapper will make no attempt to apply
         * RAII semantics.
         */
        Allocated(HandleType handle, DeviceType* device_ = nullptr);

    public:
        const HandleType* get() const { return &ParentType::get_handle(); }

    private:
        VmaAllocationCreateInfo allocation_create_info = {};
        VmaAllocation           allocation             = VK_NULL_HANDLE;

        /**
         * @brief A pointer to the allocation memory, if the memory is HOST_VISIBLE and is currently (or persistently) mapped.
         * Contains null otherwise.
         */
        uint8_t* mapped_data = nullptr;

        /**
         * @brief This flag is set to true if the memory is coherent and doesn't need to be flushed after writes.
         *
         * @note This is initialized at allocation time to avoid subsequent need to call a function to fetch the
         * allocation information from the VMA, since this property won't change for the lifetime of the allocation.
         */
        bool coherent = false;

        /**
         * @brief This flag is set to true if the memory is persistently mapped (i.e. not just HOST_VISIBLE, but available
         * as a pointer to the application for the lifetime of the allocation).
         *
         * @note This is initialized at allocation time to avoid subsequent need to call a function to fetch the
         * allocation information from the VMA, since this property won't change for the lifetime of the allocation.
         */
        bool persistent = false;
    };

    template <typename HandleType>
    inline Allocated<HandleType>::Allocated(Allocated&& other) noexcept :
        ParentType{ static_cast<ParentType&&>(other) },
        allocation_create_info(std::exchange(other.allocation_create_info, {})),
        allocation(std::exchange(other.allocation, {})),
        mapped_data(std::exchange(other.mapped_data, {})),
        coherent(std::exchange(other.coherent, {})),
        persistent(std::exchange(other.persistent, {}))
    { }

    template <typename HandleType>
    template <typename... Args>
    inline Allocated<HandleType>::Allocated(const VmaAllocationCreateInfo& allocation_create_info, Args&& ...args) :
        ParentType{ std::forward<Args>(args)... },
        allocation_create_info(allocation_create_info)
    { }

    template <typename HandleType>
    inline Allocated<HandleType>::Allocated(HandleType handle, DeviceType* device_) :
        ParentType(handle, device_)
    { }
}