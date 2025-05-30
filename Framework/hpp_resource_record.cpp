#include "stdafx.h"
#include "common/helpers.h"

namespace vkb
{
    namespace
    {
        inline void write_subpass_info(std::ostringstream& os, const std::vector<core::HPPSubpassInfo>& value)
        {
            write(os, value.size());
            for (const core::HPPSubpassInfo& item : value)
            {
                write(os, item.input_attachments);
                write(os, item.output_attachments);
            }
        }
    }

    size_t HPPResourceRecord::register_render_pass(const std::vector<rendering::HPPAttachment>& attachments,
                                                   const std::vector<HPPLoadStoreInfo>&         load_store_infos,
                                                   const std::vector<core::HPPSubpassInfo>&     subpasses)
    {
        render_pass_indices.push_back(render_pass_indices.size());

        write(stream,
            ResourceType::RenderPass,
            attachments,
            load_store_infos);

        write_subpass_info(stream, subpasses);

        return render_pass_indices.back();
    }

    void HPPResourceRecord::set_render_pass(size_t index, const core::HPPRenderPass& render_pass)
    {
        render_pass_to_index[&render_pass] = index;
    }
}