#include "stdafx.h"
#include "hpp_instance.h"

#if defined(_DEBUG)
#   define USE_VALIDATION_LAYERS
#   define USE_VALIDATION_LAYER_FEATURES
#endif

namespace vkb::core
{
	namespace
	{
		bool enable_extension(const char*								  requested_extension,
							  const std::vector<vk::ExtensionProperties>& available_extensions,
							  std::vector<const char*>&					  enabled_extensions)
		{
			bool is_available =
				std::ranges::any_of(available_extensions,
								    [&requested_extension](const auto& available_extension) { return strcmp(requested_extension, available_extension.extensionName) == 0; });
			if (is_available)
			{
				bool is_already_enabled =
					std::ranges::any_of(enabled_extensions,
										[&requested_extension](const auto& enabled_extension) { return strcmp(requested_extension, enabled_extension) == 0; });
				if (!is_already_enabled)
				{
					enabled_extensions.emplace_back(requested_extension);
				}
			}

			return is_available;
		}

		bool enable_layer(const char*							  requested_layer,
						  const std::vector<vk::LayerProperties>& available_layers,
						  std::vector<const char*>&				  enabled_layers)
		{
			bool is_available =
				std::ranges::any_of(available_layers,
									[&requested_layer](const auto& available_layer) { return strcmp(requested_layer, available_layer.layerName) == 0; });
			if (is_available)
			{
				bool is_already_enabled =
					std::ranges::any_of(enabled_layers,
										[&requested_layer](const auto& enabled_layer) { return strcmp(requested_layer, enabled_layer) == 0; });
				if (!is_already_enabled)
				{
					enabled_layers.emplace_back(requested_layer);
				}
			}

			return is_available;
		}
	}

	HPPInstance::HPPInstance(const std::string&						 application_name,
							 const std::vector<const char*>&		 requested_extensions,
							 const std::vector<const char*>&		 requested_layers,
							 const std::vector<vk::LayerSettingEXT>& required_layer_settings,
							 uint32_t								 api_version)
	{
		auto available_instance_extensions = vk::enumerateInstanceExtensionProperties();

		// Specific surface extensions are obtained from  Window::get_required_surface_extensions
		// They are already added to requested_extensions by VulkanSample::prepare

		// Even for a headless surface a swapchain is still required
		enable_extension(VK_KHR_SURFACE_EXTENSION_NAME, available_instance_extensions, enabled_extensions);

		// VK_KHR_get_physical_device_properties2 is a prerequisite of VK_KHR_performance_query
		// which will be used for stats gathering where available.
		enable_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, available_instance_extensions, enabled_extensions);

		for (auto requested_extension : requested_extensions)
		{
			if (!enable_extension(requested_extension, available_instance_extensions, enabled_extensions))
			{
				throw std::runtime_error("Required instance exntensions are missing.");
			}
		}

		auto supported_layers = vk::enumerateInstanceLayerProperties();
		std::vector<const char*> enabled_layers;

		for (auto requested_layer : requested_layers)
		{
			if (!enable_layer(requested_layer, supported_layers, enabled_layers))
			{
				throw std::runtime_error("Required layers are missing.");
			}
		}

#ifdef USE_VALIDATION_LAYERS
		// NOTE: It's important to have the validation layer as the last one here!!!!
		//			 Otherwise, device creation fails !?!
		enable_layer("VK_LAYER_KHRONOS_validation", supported_layers, enabled_layers);
#endif
	}

	HPPInstance::HPPInstance(vk::Instance instance) :
		handle{instance}
	{
	}

	HPPInstance::~HPPInstance()
	{
		if (handle)
		{
			handle.destroy();
		}
	}

	const std::vector<const char*>& HPPInstance::get_extensions()
	{
		return enabled_extensions;
	}

	vk::Instance HPPInstance::get_handle() const
	{
		return handle;
	}

	bool HPPInstance::is_enabled(const char* extension) const
	{
		return std::ranges::find_if(enabled_extensions,
									[extension](const char* enabled_extension) {return strcmp(extension, enabled_extension) == 0; }) != enabled_extensions.end();
	}
}
