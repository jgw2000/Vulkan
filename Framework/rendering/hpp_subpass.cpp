#include "stdafx.h"

namespace vkb::rendering
{
    void HPPSubpass::update_render_target_attachments(HPPRenderTarget& render_target)
    {
        render_target.set_input_attachments(input_attachments);
        render_target.set_output_attachments(output_attachments);
    }
}