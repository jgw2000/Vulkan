#pragma once

#include <cassert>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <functional>
#include <algorithm>
#include <mutex>

#include <Volk/volk.h>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_format_traits.hpp>
#include <vulkan/vulkan_hash.hpp>

#include "common/vk_common.h"
#include "platform/window.h"

#include "filesystem/legacy.h"
#include "filesystem/filesystem.h"
#include "filesystem/std_filesystem.h"

#include "core/allocated.h"
#include "core/vulkan_resource.h"
#include "core/hpp_instance.h"
#include "core/hpp_physical_device.h"
#include "core/hpp_queue.h"
#include "core/hpp_device.h"
#include "core/hpp_swapchain.h"
#include "core/hpp_image.h"
#include "core/hpp_image_view.h"
#include "core/hpp_render_pass.h"
#include "core/hpp_command_pool.h"
#include "core/hpp_command_buffer.h"
#include "core/hpp_framebuffer.h"
#include "core/hpp_pipeline_layout.h"
#include "core/hpp_shader_module.h"

#include "rendering/hpp_render_target.h"
#include "rendering/hpp_render_frame.h"
#include "rendering/hpp_render_context.h"
#include "rendering/hpp_render_pipeline.h"
#include "rendering/hpp_pipeline_state.h"
#include "rendering/hpp_subpass.h"

#include "hpp_resource_record.h"
#include "hpp_resource_cache.h"
#include "hpp_semaphore_pool.h"
#include "hpp_fence_pool.h"