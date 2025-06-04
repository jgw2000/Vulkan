#pragma once


namespace vkb
{
    enum class CommandBufferResetMode
    {
        ResetPool,
        ResetIndividually,
        AlwaysAllocate,
    };

    struct HPPImageMemoryBarrier
    {
        vk::PipelineStageFlags src_stage_mask = vk::PipelineStageFlagBits::eBottomOfPipe;
        vk::PipelineStageFlags dst_stage_mask = vk::PipelineStageFlagBits::eTopOfPipe;
        vk::AccessFlags src_access_mask;
        vk::AccessFlags dst_access_mask;
        vk::ImageLayout old_layout = vk::ImageLayout::eUndefined;
        vk::ImageLayout new_layout = vk::ImageLayout::eUndefined;
        uint32_t old_queue_family = VK_QUEUE_FAMILY_IGNORED;
        uint32_t new_queue_family = VK_QUEUE_FAMILY_IGNORED;
    };

    struct HPPLoadStoreInfo
    {
        vk::AttachmentLoadOp load_op = vk::AttachmentLoadOp::eClear;
        vk::AttachmentStoreOp store_op = vk::AttachmentStoreOp::eStore;
    };

    /**
     * @brief Helper function to determine if a Vulkan format is depth only.
     * @param format Vulkan format to check.
     * @return True if format is a depth only, false otherwise.
     */
    bool is_depth_only_format(vk::Format format);

    /**
     * @brief Helper function to determine if a Vulkan format is depth with stencil.
     * @param format Vulkan format to check.
     * @return True if format is a depth with stencil, false otherwise.
     */
    bool is_depth_stencil_format(vk::Format format);

    /**
     * @brief Helper function to determine if a Vulkan format is depth.
     * @param format Vulkan format to check.
     * @return True if format is a depth, false otherwise.
     */
    bool is_depth_format(vk::Format format);

    /**
     * @brief Helper function to determine a suitable supported depth format based on a priority list
     * @param physical_device The physical device to check the depth formats against
     * @param depth_only (Optional) Wether to include the stencil component in the format or not
     * @param depth_format_priority_list (Optional) The list of depth formats to prefer over one another
     *          By default we start with the highest precision packed format
     * @return The valid suited depth format
     */
    vk::Format get_suitable_depth_format(
        vk::PhysicalDevice             physical_device,
        bool                           depth_only = false,
        const std::vector<vk::Format>& depth_format_priority_list = {
            vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16Unorm
        }
    );

    /*
    {
        return static_cast<vk::Format>(
            vkb::get_suitable_depth_format(physical_device, depth_only, reinterpret_cast<const std::vector<VkFormat>&>(depth_format_priority_list));
        );
    }
    */
}