#pragma once

#include "core/hpp_render_pass.h"
#include "core/hpp_framebuffer.h"
#include "hpp_resource_record.h"

namespace vkb
{
    /**
     * @brief Struct to hold the internal state of the Resource Cache
     */
    struct HPPResourceCacheState
    {
        std::unordered_map<std::size_t, core::HPPRenderPass> render_passes;
        std::unordered_map<std::size_t, core::HPPFramebuffer> framebuffers;
    };

    /**
     * @brief Cache all sorts of Vulkan objects specific to a Vulkan device.
     * Supports serialization and deserialization of cached resources.
     * There is only one cache for all these objects, with several unordered_map of hash indices
     * and objects. For every object requested, there is a templated version on request_resource.
     * Some objects may need building if they are not found in the cache.
     *
     * The resource cache is also linked with ResourceRecord and ResourceReplay. Replay can warm-up
     * the cache on app startup by creating all necessary objects.
     * The cache holds pointers to objects and has a mapping from such pointers to hashes.
     * It can only be destroyed in bulk, single elements cannot be removed.
     */
    class HPPResourceCache
    {
    public:
        HPPResourceCache(vkb::core::HPPDevice& device);

        HPPResourceCache(const HPPResourceCache&)            = delete;
        HPPResourceCache(HPPResourceCache&&)                 = delete;
        HPPResourceCache& operator=(const HPPResourceCache&) = delete;
        HPPResourceCache& operator=(HPPResourceCache&&)      = delete;

        void clear();
        void clear_framebuffers();

        core::HPPRenderPass& request_render_pass(const std::vector<rendering::HPPAttachment>& attachments,
                                                 const std::vector<HPPLoadStoreInfo>&         load_store_infos,
                                                 const std::vector<core::HPPSubpassInfo>&     subpasses);
        core::HPPFramebuffer& request_framebuffer(const rendering::HPPRenderTarget& render_target, const core::HPPRenderPass& render_pass);

    private:
        vkb::core::HPPDevice&  device;
        vkb::HPPResourceRecord recorder;
        vk::PipelineCache      pipeline_cache = nullptr;
        HPPResourceCacheState  state = {};
        std::mutex             render_pass_mutex = {};
        std::mutex             framebuffer_mutex = {};
    };
}