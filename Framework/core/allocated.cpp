#include "stdafx.h"

namespace vkb::allocated
{
    VmaAllocator& get_memory_allocator()
    {
        static VmaAllocator memory_allocator = VK_NULL_HANDLE;
        return memory_allocator;
    }

    void init(const VmaAllocatorCreateInfo& create_info)
    {
        auto& allocator = get_memory_allocator();
        if (allocator == VK_NULL_HANDLE)
        {
            VkResult result = vmaCreateAllocator(&create_info, &allocator);
            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("Cannot create allocator");
            }
        }
    }

    void init(const vkb::core::HPPDevice& device)
    {
        VmaVulkanFunctions vma_vulkan_func{};
        vma_vulkan_func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vma_vulkan_func.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.pVulkanFunctions = &vma_vulkan_func;
        allocator_info.physicalDevice = static_cast<VkPhysicalDevice>(device.get_gpu().get_handle());
        allocator_info.device = static_cast<VkDevice>(device.get_handle());
        allocator_info.instance = static_cast<VkInstance>(device.get_gpu().get_instance().get_handle());

        bool can_get_memory_requirements = device.is_extension_supported(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        bool has_dedicated_allocation = device.is_extension_supported(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        if (can_get_memory_requirements && has_dedicated_allocation && device.is_enabled(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME))
        {
            allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        }

        if (device.is_extension_supported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) && device.is_enabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
        {
            allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        }

        if (device.is_extension_supported(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) && device.is_enabled(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
        {
            allocator_info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        }

        if (device.is_extension_supported(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME) && device.is_enabled(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
        {
            allocator_info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
        }

        if (device.is_extension_supported(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME) && device.is_enabled(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME))
        {
            allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
        }

        if (device.is_extension_supported(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME) && device.is_enabled(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
        {
            allocator_info.flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
        }

        init(allocator_info);
    }

    void shutdown()
    {
        auto& allocator = get_memory_allocator();
        if (allocator != VK_NULL_HANDLE)
        {
            VmaTotalStatistics stats;
            vmaCalculateStatistics(allocator, &stats);
            vmaDestroyAllocator(allocator);
            allocator = VK_NULL_HANDLE;
        }
    }
}