#include "stdafx.h"
#include "vulkan_sample.h"

namespace vkb
{
	VulkanSample::VulkanSample(const Window::Properties& properties) :
		Application(properties)
	{
	}

	VulkanSample::~VulkanSample()
	{
	}

	bool VulkanSample::prepare()
	{
		if (!Application::prepare())
		{
			return false;
		}

		if (volkInitialize() != VK_SUCCESS)
		{
			throw std::runtime_error("volkInitialize failed.");
		}

		return true;
	}

	void VulkanSample::update(float delta_time)
	{
		Application::update(delta_time);
	}

	void VulkanSample::finish()
	{
		Application::finish();
	}

	bool VulkanSample::resize(uint32_t width, uint32_t height)
	{
		if (!Application::resize(width, height))
		{
			return false;
		}

		return true;
	}
}
