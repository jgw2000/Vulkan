#pragma once

namespace vkb::core
{
    class HPPDevice;

    /**
     * @brief A wrapper class for vk::Queue
     *
     */
    class HPPQueue
    {
    public:
        HPPQueue(HPPDevice& device, uint32_t family_index, vk::QueueFamilyProperties properties, vk::Bool32 can_present, uint32_t index);
        HPPQueue(const HPPQueue&) = default;
        HPPQueue(HPPQueue&& other);

        HPPQueue& operator=(const HPPQueue&) = delete;
        HPPQueue& operator=(HPPQueue&&) = delete;

        const HPPDevice& get_device() const { return device; }
        vk::Queue get_handle() const { return handle; }
        uint32_t get_family_index() const { return family_index; }
        uint32_t get_index() const { return index; }
        const vk::QueueFamilyProperties& get_properties() const { return properties; }
        vk::Bool32 support_present() const { return can_present; }


    private:
        HPPDevice& device;

        vk::Queue handle;

        uint32_t family_index{ 0 };

        uint32_t index{ 0 };

        vk::Bool32 can_present = false;

        vk::QueueFamilyProperties properties{};
    };
}
