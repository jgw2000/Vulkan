#pragma once

#include "hpp_shader_module.h"

namespace vkb::core
{
    class HPPDevice;

    class HPPPipelineLayout
    {
    public:
        HPPPipelineLayout(HPPDevice& device, const std::vector<HPPShaderModule*>& shader_modules);
        ~HPPPipelineLayout();

        HPPPipelineLayout(const HPPPipelineLayout&) = delete;
        HPPPipelineLayout(HPPPipelineLayout&& other);

        HPPPipelineLayout& operator=(const HPPPipelineLayout&) = delete;
        HPPPipelineLayout& operator=(HPPPipelineLayout&&) = delete;

        vk::PipelineLayout                                                  get_handle() const         { return handle; }
        const std::vector<HPPShaderModule*>&                                get_shader_modules() const { return shader_modules; }
        const std::unordered_map<uint32_t, std::vector<HPPShaderResource>>& get_shader_sets() const    { return shader_sets; }

    private:
        HPPDevice&                                                   device;
        vk::PipelineLayout                                           handle;
        std::vector<HPPShaderModule*>                                shader_modules;        // The shader modules that this pipeline layout uses
        std::unordered_map<std::string, HPPShaderResource>           shader_resources;      // The shader resources that this pipeline layout uses, indexed by their name
        std::unordered_map<uint32_t, std::vector<HPPShaderResource>> shader_sets;           // A map of each set and the resources it owns used by the pipeline layout
    };
}