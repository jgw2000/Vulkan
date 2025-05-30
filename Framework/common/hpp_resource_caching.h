#pragma once

#include "helpers.h"

namespace std
{
    template <typename Key, typename Value>
    struct hash<std::map<Key, Value>>
    {
        size_t operator()(const std::map<Key, Value>& bindings) const
        {
            size_t result = 0;
            vkb::hash_combine(result, bindings.size());
            for (const auto& binding : bindings)
            {
                vkb::hash_combine(result, binding.first);
                vkb::hash_combine(result, binding.second);
            }
            return result;
        }
    };

    template <typename T>
    struct hash <std::vector<T>>
    {
        size_t operator()(const std::vector<T>& values) const
        {
            size_t result = 0;
            vkb::hash_combine(result, values.size());
            for (const auto& value : values)
            {
                vkb::hash_combine(result, value);
            }
            return result;
        }
    };

    template <>
    struct hash<vkb::HPPLoadStoreInfo>
    {
        size_t operator()(const vkb::HPPLoadStoreInfo& lsi) const
        {
            size_t result = 0;
            vkb::hash_combine(result, lsi.load_op);
            vkb::hash_combine(result, lsi.store_op);
            return result;
        }
    };

    template <typename T>
    struct hash<vkb::core::VulkanResource<T>>
    {
        size_t operator()(const vkb::core::VulkanResource<T>& vulkan_resource) const
        {
            return std::hash<T>()(vulkan_resource.get_handle());
        }
    };

    template <>
    struct hash<vkb::core::HPPSubpassInfo>
    {
        size_t operator()(const vkb::core::HPPSubpassInfo& subpass_info) const
        {
            size_t result = 0;
            vkb::hash_combine(result, subpass_info.input_attachments);
            vkb::hash_combine(result, subpass_info.output_attachments);
            vkb::hash_combine(result, subpass_info.color_resolve_attachments);
            vkb::hash_combine(result, subpass_info.disable_depth_stencil_attachment);
            vkb::hash_combine(result, subpass_info.depth_stencil_resolve_attachment);
            vkb::hash_combine(result, subpass_info.depth_stencil_resolve_mode);
            return result;
        }
    };

    template <>
    struct hash<vkb::rendering::HPPAttachment>
    {
        size_t operator()(const vkb::rendering::HPPAttachment& attachment) const
        {
            size_t result = 0;
            vkb::hash_combine(result, attachment.format);
            vkb::hash_combine(result, attachment.samples);
            vkb::hash_combine(result, attachment.usage);
            vkb::hash_combine(result, attachment.initial_layout);
            return result;
        }
    };

    template <>
    struct hash<vkb::core::HPPRenderPass>
    {
        size_t operator()(const vkb::core::HPPRenderPass& render_pass) const
        {
            size_t result = 0;
            vkb::hash_combine(result, render_pass.get_handle());
            return result;
        }
    };

    template <>
    struct hash<vkb::core::HPPImage>
    {
        size_t operator()(const vkb::core::HPPImage& image) const
        {
            size_t result = 0;
            vkb::hash_combine(result, image.get_memory());
            vkb::hash_combine(result, image.get_type());
            vkb::hash_combine(result, image.get_extent());
            vkb::hash_combine(result, image.get_format());
            vkb::hash_combine(result, image.get_usage());
            vkb::hash_combine(result, image.get_sample_count());
            vkb::hash_combine(result, image.get_tiling());
            vkb::hash_combine(result, image.get_subresource());
            vkb::hash_combine(result, image.get_array_layer_count());
            return result;
        }
    };

    template <>
    struct hash<vkb::core::HPPImageView>
    {
        size_t operator()(const vkb::core::HPPImageView& image_view) const
        {
            size_t result = std::hash<vkb::core::VulkanResource<vk::ImageView>>()(image_view);
            vkb::hash_combine(result, image_view.get_image());
            vkb::hash_combine(result, image_view.get_format());
            vkb::hash_combine(result, image_view.get_subresource_range());
            return result;
        }
    };

    template <>
    struct hash<vkb::rendering::HPPRenderTarget>
    {
        size_t operator()(const vkb::rendering::HPPRenderTarget& render_target) const
        {
            size_t result = 0;
            
            for (auto& view : render_target.get_views())
            {
                vkb::hash_combine(result, view.get_handle());
                vkb::hash_combine(result, view.get_image().get_handle());
            }

            return result;
        }
    };
}

namespace vkb::common
{
    namespace
    {
        template <typename T>
        inline void hash_param(size_t& seed, const T& value)
        {
            hash_combine(seed, value);
        }

        template <typename T, typename... Args>
        inline void hash_param(size_t& seed, const T& first_arg, const Args&... args)
        {
            hash_param(seed, first_arg);
            hash_param(seed, args...);
        }

        template <>
        inline void hash_param<std::vector<vkb::core::HPPSubpassInfo>>(
            size_t& seed,
            const std::vector<vkb::core::HPPSubpassInfo>& value)
        {
            for (auto& subpass_info : value)
            {
                hash_combine(seed, subpass_info);
            }
        }

        template <>
        inline void hash_param<std::vector<HPPLoadStoreInfo>>(
            size_t& seed,
            const std::vector<HPPLoadStoreInfo>& value)
        {
            for (auto& load_store_info : value)
            {
                hash_combine(seed, load_store_info);
            }
        }

        template <>
        inline void hash_param<std::vector<rendering::HPPAttachment>>(
            size_t& seed,
            const std::vector<rendering::HPPAttachment>& value)
        {
            for (auto& attachment : value)
            {
                hash_combine(seed, attachment);
            }
        }

        template <>
        inline void hash_param<std::vector<uint8_t>>(
            size_t& seed,
            const std::vector<uint8_t>& value)
        {
            hash_combine(seed, std::string{ value.begin(), value.end() });
        }

        template<class T, class... A>
        struct HPPRecordHelper
        {
            size_t record(HPPResourceRecord&, A&...)
            {
                return 0;
            }

            void index(HPPResourceRecord&, size_t, T&)
            { }
        };

        template <class... A>
        struct HPPRecordHelper<vkb::core::HPPRenderPass, A...>
        {
            size_t record(HPPResourceRecord& recorder, A&... args)
            {
                return recorder.register_render_pass(args...);
            }

            void index(HPPResourceRecord& recorder, size_t index, vkb::core::HPPRenderPass& render_pass)
            {
                return recorder.set_render_pass(index, render_pass);
            }
        };
    }

    template <class T, class... A>
    T& request_resource(vkb::core::HPPDevice& device, vkb::HPPResourceRecord* recorder, std::unordered_map<size_t, T>& resources, A&... args)
    {
        HPPRecordHelper<T, A...> record_helper;

        size_t hash{ 0U };
        hash_param(hash, args...);

        auto res_it = resources.find(hash);
        if (res_it != resources.end())
        {
            return res_it->second;
        }

        // If we do not have it already, create and cache it
        const char* res_type = typeid(T).name();
        size_t res_id = resources.size();

// Only error hande in release
#ifndef DEBUG
        try
        {
#endif
            T resource(device, args...);

            auto res_ins_it = resources.emplace(hash, std::move(resource));
            if (!res_ins_it.second)
            {
                throw std::runtime_error{ std::string{"Insertion error for #"} + std::to_string(res_id) + "cache object (" + res_type + ")" };
            }

            res_it = res_ins_it.first;

            if (recorder)
            {
                size_t index = record_helper.record(*recorder, args...);
                record_helper.index(*recorder, index, res_it->second);
            }
#ifndef DEBUG
        }
        catch (const std::exception& e)
        {
            throw e;
        }
#endif
        return res_it->second;
    }
}