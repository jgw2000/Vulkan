#include "stdafx.h"

namespace vkb::core
{
    HPPPipelineLayout::HPPPipelineLayout(HPPDevice& device, const std::vector<HPPShaderModule*>& shader_modules) :
        device{ device },
        shader_modules{ shader_modules }
    {
        // Collect and combine all the shader resources from each of the shader modules
        // Collate them all into a map that is indexed by the name of the resource
        for (auto* shader_module : shader_modules)
        {
            for (const auto& shader_resource : shader_module->get_resources())
            {
                std::string key = shader_resource.name;

                // Since 'Input' and 'Output' resources can have the same name, we modify the key string
                if (shader_resource.type == HPPShaderResourceType::Input || shader_resource.type == HPPShaderResourceType::Output)
                {
                    key = std::to_string(static_cast<uint32_t>(shader_resource.stages)) + "_" + key;
                }

                auto it = shader_resources.find(key);

                if (it != shader_resources.end())
                {
                    // Append stage flags if resource already exists
                    it->second.stages |= shader_resource.stages;
                }
                else
                {
                    // Create a new entry in the map
                    shader_resources.emplace(key, shader_resource);
                }
            }
        }

        // Sift through the map of name indexed shader resources
        // Separate them into their respective sets
        for (auto& it : shader_resources)
        {
            auto& shader_resource = it.second;

            // Find binding by set index in the map
            auto it2 = shader_sets.find(shader_resource.set);

            if (it2 != shader_sets.end())
            {
                // Add resource to the found set index
                it2->second.push_back(shader_resource);
            }
            else
            {
                // Create a new set index and with the first resource
                shader_sets.emplace(shader_resource.set, std::vector<HPPShaderResource>{shader_resource});
            }
        }

        // TODO

        vk::PipelineLayoutCreateInfo create_info = {};

        // Create the Vulkan pipeline layout handle
        handle = device.get_handle().createPipelineLayout(create_info);
    }

    HPPPipelineLayout::HPPPipelineLayout(HPPPipelineLayout&& other) :
        device{ other.device },
        handle{ other.handle },
        shader_modules{ std::move(other.shader_modules) },
        shader_resources{ std::move(other.shader_resources) },
        shader_sets{ std::move(other.shader_sets) }
    {
        other.handle = nullptr;
    }

    HPPPipelineLayout::~HPPPipelineLayout()
    {
        // Destroy pipeline layout
        if (handle)
        {
            device.get_handle().destroyPipelineLayout(handle);
        }
    }
}