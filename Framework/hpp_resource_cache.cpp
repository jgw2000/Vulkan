#include "stdafx.h"

namespace vkb
{
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
}