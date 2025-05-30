#include "stdafx.h"
#include "common/hpp_resource_caching.h"

namespace vkb
{
    namespace
    {
        template <class T, class... A>
        T& request_resource(
            core::HPPDevice& device, HPPResourceRecord& recorder, std::mutex& resource_mutex, std::unordered_map<std::size_t, T>& resources, A&... args)
        {
            std::lock_guard<std::mutex> guard(resource_mutex);

            return common::request_resource(device, &recorder, resources, args...);
        }
    }

    HPPResourceCache::HPPResourceCache(vkb::core::HPPDevice& device) :
        device{device}
    { }

    void HPPResourceCache::clear()
    {
        // TODO
    }

    void HPPResourceCache::clear_framebuffers()
    {
        state.framebuffers.clear();
    }

    core::HPPRenderPass& HPPResourceCache::request_render_pass(const std::vector<rendering::HPPAttachment>& attachments,
                                                               const std::vector<HPPLoadStoreInfo>&         load_store_infos,
                                                               const std::vector<core::HPPSubpassInfo>&     subpasses)
    {
        return request_resource(device, recorder, render_pass_mutex, state.render_passes, attachments, load_store_infos, subpasses);
    }

    core::HPPFramebuffer& HPPResourceCache::request_framebuffer(const rendering::HPPRenderTarget& render_target, const core::HPPRenderPass& render_pass)
    {
        return request_resource(device, recorder, framebuffer_mutex, state.framebuffers, render_target, render_pass);
    }
}