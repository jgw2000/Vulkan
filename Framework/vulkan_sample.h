#pragma once

namespace vkb
{
    /**
     * @mainpage Overview of the framework
     *
     * @section initialization Initialization
     *
     * @subsection platform_init Platform initialization
     * The lifecycle of a Vulkan sample starts by instantiating the correct Platform
     * (e.g. WindowsPlatform) and then calling initialize() on it, which sets up
     * the windowing system and logging. Then it calls the parent Platform::initialize(),
     * which takes ownership of the active application. It's the platforms responsibility
     * to then call VulkanSample::prepare() to prepare the vulkan sample when it is ready.
     *
     * @subsection sample_init Sample initialization
     * The preparation step is divided in two steps, one in VulkanSample and the other in the
     * specific sample, such as SurfaceRotation.
     * VulkanSample::prepare() contains functions that do not require customization,
     * including creating a Vulkan instance, the surface and getting physical devices.
     * The prepare() function for the specific sample completes the initialization, including:
     * - setting enabled Stats
     * - creating the Device
     * - creating the Swapchain
     * - creating the RenderContext (or child class)
     * - preparing the RenderContext
     * - loading the sg::Scene
     * - creating the RenderPipeline with ShaderModule (s)
     * - creating the sg::Camera
     * - creating the Gui
     *
     * @section frame_rendering Frame rendering
     *
     * @subsection update Update function
     * Rendering happens in the update() function. Each sample can override it, e.g.
     * to recreate the Swapchain in SwapchainImages when required by user input.
     * Typically a sample will then call VulkanSample::update().
     *
     * @subsection rendering Rendering
     * A series of steps are performed, some of which can be customized (it will be
     * highlighted when that's the case):
     *
     * - calling sg::Script::update() for all sg::Script (s)
     * - beginning a frame in RenderContext (does the necessary waiting on fences and
     *   acquires an core::Image)
     * - requesting a CommandBuffer
     * - updating Stats and Gui
     * - getting an active RenderTarget constructed by the factory function of the RenderFrame
     * - setting up barriers for color and depth, note that these are only for the default RenderTarget
     * - calling VulkanSample::draw_swapchain_renderpass (see below)
     * - setting up a barrier for the Swapchain transition to present
     * - submitting the CommandBuffer and end the Frame (present)
     *
     * @subsection draw_swapchain Draw swapchain renderpass
     * The function starts and ends a RenderPass which includes setting up viewport, scissors,
     * blend state (etc.) and calling draw_scene.
     * Note that RenderPipeline::draw is not virtual in RenderPipeline, but internally it calls
     * Subpass::draw for each Subpass, which is virtual and can be customized.
     *
     * @section framework_classes Main framework classes
     *
     * - RenderContext
     * - RenderFrame
     * - RenderTarget
     * - RenderPipeline
     * - ShaderModule
     * - ResourceCache
     * - BufferPool
     * - Core classes: Classes in vkb::core wrap Vulkan objects for indexing and hashing.
     */
}