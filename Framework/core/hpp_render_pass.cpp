#include "stdafx.h"

namespace vkb::core
{
    HPPRenderPass::HPPRenderPass(HPPDevice&                                        device,
                                 const std::vector<vkb::rendering::HPPAttachment>& attachments,
                                 const std::vector<vkb::HPPLoadStoreInfo>& load_store_infos,
                                 const std::vector<HPPSubpassInfo>&                subpasses) :
        VulkanResource{ nullptr, &device },
        subpass_count{ std::max<size_t>(1, subpasses.size()) },       // At least 1 subpass
        color_output_count{}
    {
        if (device.is_enabled(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
        {
            create_renderpass<vk::SubpassDescription2KHR, vk::AttachmentDescription2KHR, vk::AttachmentReference2KHR, vk::SubpassDependency2KHR, vk::RenderPassCreateInfo2KHR>(
                attachments, load_store_infos, subpasses
            );
        }
        else
        {
            create_renderpass<vk::SubpassDescription, vk::AttachmentDescription, vk::AttachmentReference, vk::SubpassDependency, vk::RenderPassCreateInfo>(
                attachments, load_store_infos, subpasses
            );
        }
    }

    HPPRenderPass::~HPPRenderPass()
    {
        // Destroy render pass
        if (has_device())
        {
            get_device().get_handle().destroyRenderPass(get_handle());
        }
    }

    HPPRenderPass::HPPRenderPass(HPPRenderPass&& other) :
        VulkanResource{ std::move(other) },
        subpass_count{ other.subpass_count },
        color_output_count{ other.color_output_count }
    { }

    const uint32_t HPPRenderPass::get_color_output_count(uint32_t subpass_index) const
    {
        return color_output_count[subpass_index];
    }

    vk::Extent2D HPPRenderPass::get_render_area_granularity() const
    {
        return get_device().get_handle().getRenderAreaGranularity(get_handle());
    }

    template <typename T>
    std::vector<T> get_attachment_descriptions(const std::vector<vkb::rendering::HPPAttachment>& attachments, const std::vector<vkb::HPPLoadStoreInfo>& load_store_infos)
    {
        std::vector<T> attachment_descriptions;

        for (size_t i = 0U; i < attachments.size(); ++i)
        {
            T attachment{};

            attachment.format = attachments[i].format;
            attachment.samples = attachments[i].samples;
            attachment.initialLayout = attachments[i].initial_layout;
            attachment.finalLayout =
                vkb::is_depth_format(attachment.format) ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal;

            if (i < load_store_infos.size())
            {
                attachment.loadOp = load_store_infos[i].load_op;
                attachment.storeOp = load_store_infos[i].store_op;
                attachment.stencilLoadOp = load_store_infos[i].load_op;
                attachment.stencilStoreOp = load_store_infos[i].store_op;
            }

            attachment_descriptions.push_back(std::move(attachment));
        }

        return attachment_descriptions;
    }

    template <typename T>
    T get_attachment_reference(const uint32_t attachment, const vk::ImageLayout layout)
    {
        T reference{};

        reference.attachment = attachment;
        reference.layout = layout;

        return reference;
    }

    inline void set_pointer_next(vk::SubpassDescription& subpass_description, vk::SubpassDescriptionDepthStencilResolveKHR& depth_resolve, vk::AttachmentReference& depth_resolve_attachment)
    {
    }

    inline void set_pointer_next(vk::SubpassDescription2KHR& subpass_description, vk::SubpassDescriptionDepthStencilResolveKHR& depth_resolve, vk::AttachmentReference2KHR& depth_resolve_attachment)
    {
        depth_resolve.pDepthStencilResolveAttachment = &depth_resolve_attachment;
        subpass_description.pNext                    = &depth_resolve;
    }

    inline const vk::AttachmentReference2KHR* get_depth_resolve_reference(const vk::SubpassDescription& subpass_description)
    {
        return nullptr;
    }

    inline const vk::AttachmentReference2KHR* get_depth_resolve_reference(const vk::SubpassDescription2KHR& subpass_description)
    {
        auto description_depth_resolve = static_cast<const vk::SubpassDescriptionDepthStencilResolveKHR*>(subpass_description.pNext);

        const vk::AttachmentReference2KHR* depth_resolve_attachment = nullptr;
        if (description_depth_resolve)
        {
            depth_resolve_attachment = description_depth_resolve->pDepthStencilResolveAttachment;
        }

        return depth_resolve_attachment;
    }

    template <typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference>
    void set_attachment_layouts(std::vector<T_SubpassDescription>& subpass_descriptions, std::vector<T_AttachmentDescription>& attachment_descriptions)
    {
        // Make the initial layout same as in the first subpass using that attachment
        for (auto& subpass : subpass_descriptions)
        {
            for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
            {
                auto& reference = subpass.pColorAttachments[k];
                // Set it only if not defined yet
                if (attachment_descriptions[reference.attachment].initialLayout == vk::ImageLayout::eUndefined)
                {
                    attachment_descriptions[reference.attachment].initialLayout = reference.layout;
                }
            }

            for (size_t k = 0U; k < subpass.inputAttachmentCount; ++k)
            {
                auto& reference = subpass.pInputAttachments[k];
                // Set it only if not defined yet
                if (attachment_descriptions[reference.attachment].initialLayout == vk::ImageLayout::eUndefined)
                {
                    attachment_descriptions[reference.attachment].initialLayout = reference.layout;
                }
            }

            if (subpass.pDepthStencilAttachment)
            {
                auto& reference = *subpass.pDepthStencilAttachment;
                // Set it only if not defined yet
                if (attachment_descriptions[reference.attachment].initialLayout == vk::ImageLayout::eUndefined)
                {
                    attachment_descriptions[reference.attachment].initialLayout = reference.layout;
                }
            }

            if (subpass.pResolveAttachments)
            {
                for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
                {
                    auto& reference = subpass.pResolveAttachments[k];
                    // Set it only if not defined yet
                    if (attachment_descriptions[reference.attachment].initialLayout == vk::ImageLayout::eUndefined)
                    {
                        attachment_descriptions[reference.attachment].initialLayout = reference.layout;
                    }
                }
            }

            if (const auto depth_resolve = get_depth_resolve_reference(subpass))
            {
                // Set it only if not defined yet
                if (attachment_descriptions[depth_resolve->attachment].initialLayout == vk::ImageLayout::eUndefined)
                {
                    attachment_descriptions[depth_resolve->attachment].initialLayout = depth_resolve->layout;
                }
            }
        }

        // Make the final layout same as the last subpass layout
        {
            auto& subpass = subpass_descriptions.back();

            for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
            {
                const auto& reference = subpass.pColorAttachments[k];
                attachment_descriptions[reference.attachment].finalLayout = reference.layout;
            }

            for (size_t k = 0U; k < subpass.inputAttachmentCount; ++k)
            {
                const auto& reference = subpass.pInputAttachments[k];
                attachment_descriptions[reference.attachment].finalLayout = reference.layout;

                // Do not use depth attachment if used as input
                if (vkb::is_depth_format(attachment_descriptions[reference.attachment].format))
                {
                    subpass.pDepthStencilAttachment = nullptr;
                }
            }

            if (subpass.pDepthStencilAttachment)
            {
                const auto& reference = *subpass.pDepthStencilAttachment;
                attachment_descriptions[reference.attachment].finalLayout = reference.layout;
            }

            if (subpass.pResolveAttachments)
            {
                for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
                {
                    auto& reference = subpass.pResolveAttachments[k];
                    attachment_descriptions[reference.attachment].finalLayout = reference.layout;
                }
            }

            if (const auto depth_resolve = get_depth_resolve_reference(subpass))
            {
                attachment_descriptions[depth_resolve->attachment].finalLayout = depth_resolve->layout;
            }
        }
    }

    template <typename T>
    std::vector<T> get_subpass_dependencies(const size_t subpass_count, bool depth_stencil_dependency)
    {
        std::vector<T> dependencies{};

        if (subpass_count > 1)
        {
            for (uint32_t subpass_id = 0; subpass_id < static_cast<uint32_t>(subpass_count - 1); ++subpass_id)
            {
                /**
                 * srcSubpass    - Index of a first (previous) subpass or VK_SUBPASS_EXTERNAL
                 * dstSubpass    - Index of a second (later) subpass or VK_SUBPASS_EXTERNAL
                 * srcStageMask  - What pipeline stage must have completed for the dependency
                 * dstStageMask  - What pipeline stage is waiting on the dependency
                 * srcAccessMask - What access scopes are influence the dependency
                 * dstAccessMask - What access scopes are waiting on the dependency
                 */
                T color_dep{};
                color_dep.srcSubpass = subpass_id;
                color_dep.dstSubpass = subpass_id + 1;
                color_dep.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                color_dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eFragmentShader;
                color_dep.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                color_dep.dstAccessMask = vk::AccessFlagBits::eInputAttachmentRead | vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
                color_dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;
                dependencies.push_back(color_dep);

                if (depth_stencil_dependency)
                {
                    T depth_dep{};
                    depth_dep.srcSubpass = subpass_id;
                    depth_dep.dstSubpass = subpass_id + 1;
                    depth_dep.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
                    depth_dep.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests | vk::PipelineStageFlagBits::eFragmentShader;
                    depth_dep.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    depth_dep.dstAccessMask = vk::AccessFlagBits::eInputAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    depth_dep.dependencyFlags = vk::DependencyFlagBits::eByRegion;
                    dependencies.push_back(depth_dep);
                }
            }
        }

        return dependencies;
    }

    /**
     * @brief Assuming there is only one depth attachment
     */
    template <typename T_SubpassDescription, typename T_AttachmentDescription>
    bool is_depth_a_dependency(std::vector<T_SubpassDescription>& subpass_descriptions, std::vector<T_AttachmentDescription>& attachment_descriptions)
    {
        // More than 1 subpass uses depth
        if (std::ranges::count_if(subpass_descriptions,
                                  [](const auto& subpass) {
                                      return subpass.pDepthStencilAttachment != nullptr;
                                  }) > 1)
        {
            return true;
        }

        // Otherwise check if any uses depth as an input
        return std::ranges::any_of(
            subpass_descriptions,
            [&attachment_descriptions](auto const& subpass) {
                return std::ranges::any_of(
                    std::span{ subpass.pInputAttachments, subpass.inputAttachmentCount },
                    [&attachment_descriptions](auto const& reference) {
                        return vkb::is_depth_format(attachment_descriptions[reference.attachment].format);
                    });
            });

        return false;
    }

    inline vk::RenderPass create_vk_renderpass(vk::Device device, vk::RenderPassCreateInfo& create_info)
    {
        return device.createRenderPass(create_info);
    }

    inline vk::RenderPass create_vk_renderpass(vk::Device device, vk::RenderPassCreateInfo2KHR& create_info)
    {
        return device.createRenderPass2KHR(create_info);
    }

    template <typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference, typename T_SubpassDependency, typename T_RenderPassCreateInfo>
    void HPPRenderPass::create_renderpass(
        const std::vector<vkb::rendering::HPPAttachment>& attachments,
        const std::vector<vkb::HPPLoadStoreInfo>& load_store_infos,
        const std::vector<HPPSubpassInfo>& subpasses
    )
    {
        auto attachment_descriptions = get_attachment_descriptions<T_AttachmentDescription>(attachments, load_store_infos);

        // Store attachments for every subpass
        std::vector<std::vector<T_AttachmentReference>> input_attachments{ subpass_count };
        std::vector<std::vector<T_AttachmentReference>> color_attachments{ subpass_count };
        std::vector<std::vector<T_AttachmentReference>> depth_stencil_attachments{ subpass_count };
        std::vector<std::vector<T_AttachmentReference>> color_resolve_attachments{ subpass_count };
        std::vector<std::vector<T_AttachmentReference>> depth_resolve_attachments{ subpass_count };

        for (size_t i = 0; i < subpasses.size(); ++i)
        {
            auto& subpass = subpasses[i];

            // Fill color attachments references
            for (auto o_attachment : subpass.output_attachments)
            {
                auto initial_layout = attachments[o_attachment].initial_layout == vk::ImageLayout::eUndefined ? vk::ImageLayout::eColorAttachmentOptimal : attachments[o_attachment].initial_layout;
                auto& description = attachment_descriptions[o_attachment];
                if (!vkb::is_depth_format(description.format))
                {
                    color_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(o_attachment, initial_layout));
                }
            }

            // Fill input attachments references
            for (auto i_attachment : subpass.input_attachments)
            {
                auto initial_layout = vkb::is_depth_format(attachments[i_attachment].format) ? vk::ImageLayout::eDepthStencilReadOnlyOptimal : vk::ImageLayout::eShaderReadOnlyOptimal;
                input_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(i_attachment, initial_layout));
            }

            for (auto r_attachment : subpass.color_resolve_attachments)
            {
                auto initial_layout = attachments[r_attachment].initial_layout == vk::ImageLayout::eUndefined ? vk::ImageLayout::eColorAttachmentOptimal : attachments[r_attachment].initial_layout;
                color_resolve_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(r_attachment, initial_layout));
            }

            if (!subpass.disable_depth_stencil_attachment)
            {
                // Assumption: depth stencil attachment appears in the list before any depth stencil resolve attachment
                auto it = std::ranges::find_if(attachments, [](const vkb::rendering::HPPAttachment attachment) {return vkb::is_depth_format(attachment.format); });
                if (it != attachments.end())
                {
                    auto i_depth_stencil = static_cast<uint32_t>(std::distance(attachments.begin(), it));
                    auto initial_layout = it->initial_layout == vk::ImageLayout::eUndefined ? vk::ImageLayout::eDepthStencilAttachmentOptimal : it->initial_layout;
                    depth_stencil_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(i_depth_stencil, initial_layout));

                    if (subpass.depth_stencil_resolve_mode != vk::ResolveModeFlagBits::eNone)
                    {
                        auto i_depth_stencil_resolve_mode = subpass.depth_stencil_resolve_attachment;
                        initial_layout = attachments[i_depth_stencil_resolve_mode].initial_layout == vk::ImageLayout::eUndefined ? vk::ImageLayout::eDepthStencilAttachmentOptimal : attachments[i_depth_stencil_resolve_mode].initial_layout;
                        depth_resolve_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(i_depth_stencil_resolve_mode, initial_layout));
                    }
                }
            }
        }

        std::vector<T_SubpassDescription> subpass_descriptions;
        subpass_descriptions.reserve(subpass_count);
        vk::SubpassDescriptionDepthStencilResolveKHR depth_resolve{};
        for (size_t i = 0; i < subpasses.size(); ++i)
        {
            auto& subpass = subpasses[i];

            T_SubpassDescription subpass_description{};
            
            subpass_description.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass_description.pInputAttachments       = input_attachments[i].empty() ? nullptr : input_attachments[i].data();
            subpass_description.inputAttachmentCount    = static_cast<uint32_t>(input_attachments[i].size());
            subpass_description.pColorAttachments       = color_attachments[i].empty() ? nullptr : color_attachments[i].data();
            subpass_description.colorAttachmentCount    = static_cast<uint32_t>(color_attachments[i].size());
            subpass_description.pResolveAttachments     = color_resolve_attachments[i].empty() ? nullptr : color_resolve_attachments[i].data();
            subpass_description.pDepthStencilAttachment = nullptr;

            if (!depth_stencil_attachments[i].empty())
            {
                subpass_description.pDepthStencilAttachment = depth_stencil_attachments[i].data();

                if (!depth_resolve_attachments[i].empty())
                {
                    // If the pNext list of VkSubpassDescription2 includes a VkSubpassDescriptionDepthStencilResolve structure,
                    // then that structure describes multisample resolve operations for the depth/stencil attachment in a subpass
                    depth_resolve.depthResolveMode = subpass.depth_stencil_resolve_mode;
                    set_pointer_next(subpass_description, depth_resolve, depth_resolve_attachments[i][0]);

                    auto& reference = depth_resolve_attachments[i][0];
                    // Set it only if not defined yet
                    if (attachment_descriptions[reference.attachment].initialLayout == vk::ImageLayout::eUndefined)
                    {
                        attachment_descriptions[reference.attachment].initialLayout = reference.layout;
                    }
                }
            }

            subpass_descriptions.push_back(subpass_description);
        }

        // Default subpass
        if (subpasses.empty())
        {
            T_SubpassDescription subpass_description{};
            subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

            uint32_t default_depth_stencil_attachment{ VK_ATTACHMENT_UNUSED };

            for (uint32_t k = 0U; k < static_cast<uint32_t>(attachment_descriptions.size()); ++k)
            {
                if (vkb::is_depth_format(attachments[k].format))
                {
                    if (default_depth_stencil_attachment == VK_ATTACHMENT_UNUSED)
                    {
                        default_depth_stencil_attachment = k;
                    }
                    continue;
                }

                color_attachments[0].push_back(get_attachment_reference<T_AttachmentReference>(k, vk::ImageLayout::eGeneral));
            }

            subpass_description.pColorAttachments = color_attachments[0].data();

            if (default_depth_stencil_attachment != VK_ATTACHMENT_UNUSED)
            {
                depth_stencil_attachments[0].push_back(get_attachment_reference<T_AttachmentReference>(default_depth_stencil_attachment, vk::ImageLayout::eDepthStencilAttachmentOptimal));
                
                subpass_description.pDepthStencilAttachment = depth_stencil_attachments[0].data();
            }

            subpass_descriptions.push_back(subpass_description);
        }

        set_attachment_layouts<T_SubpassDescription, T_AttachmentDescription, T_AttachmentReference>(subpass_descriptions, attachment_descriptions);

        color_output_count.reserve(subpass_count);
        for (size_t i = 0; i < subpass_count; ++i)
        {
            color_output_count.push_back(static_cast<uint32_t>(color_attachments[i].size()));
        }

        const auto& subpass_dependencies = get_subpass_dependencies<T_SubpassDependency>(subpass_count, is_depth_a_dependency(subpass_descriptions, attachment_descriptions));

        T_RenderPassCreateInfo create_info{};
        create_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
        create_info.pAttachments = attachment_descriptions.data();
        create_info.subpassCount = static_cast<uint32_t>(subpass_descriptions.size());
        create_info.pSubpasses = subpass_descriptions.data();
        create_info.dependencyCount = static_cast<uint32_t>(subpass_dependencies.size());
        create_info.pDependencies = subpass_dependencies.data();

        set_handle(create_vk_renderpass(get_device().get_handle(), create_info));
    }
}