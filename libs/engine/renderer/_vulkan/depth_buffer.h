#ifndef _src_renderer_vulkan_depth_buffer_h_
#define _src_renderer_vulkan_depth_buffer_h_

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class VulkanContext;

	class VulkanDepthBuffer
	{
	public:
		VulkanDepthBuffer(const VulkanContext&, uint32_t width, uint32_t height, VkFormat);
		~VulkanDepthBuffer() noexcept;

		VkAttachmentDescription attachment_description() const noexcept;
		VkImageView view() const noexcept { return _view; }

	private:
		const VulkanContext& _context;
		const VkFormat _format;
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;
	};
}

#endif
