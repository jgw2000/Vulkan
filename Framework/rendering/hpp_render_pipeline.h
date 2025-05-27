#pragma once

namespace vkb::rendering
{
    /**
     * @brief A RenderPipeline is a sequence of Subpass objects.
     * Subpass holds shaders and can draw the core::sg::Scene.
     * More subpasses can be added to the sequence if required.
     * For example, postprocessing can be implemented with two pipelines which
     * share render targets.
     *
     * GeometrySubpass -> Processes Scene for Shaders, use by itself if shader requires no lighting
     * ForwardSubpass -> Binds lights at the beginning of a GeometrySubpass to create Forward Rendering, should be used with most default shaders
     * LightingSubpass -> Holds a Global Light uniform, Can be combined with GeometrySubpass to create Deferred Rendering
     */
    class HPPRenderPipeline
    {

    };
}