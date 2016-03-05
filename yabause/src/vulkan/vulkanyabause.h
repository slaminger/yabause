#include <vulkan/vulkan.h>
#include "vulkanbase.h"


class VulkanYabause : public VulkanBase
{
public:
	VulkanYabause();
	~VulkanYabause();
	int yabauseinit();
	void draw();
	void prepare();
	virtual void render();
	virtual void viewChanged();

protected:
	VkSemaphore presentCompleteSemaphore;

public:	
	VkSemaphore beginMainFrame();
	int getBasicObjects(VkPhysicalDevice & physicalDevice, VkDevice & device, VkQueue & queue, VkCommandPool & cmdPool);
	VkRenderPass getRenderPass();
	VkFramebuffer getCurrentFrameBuffer();
	VkImage getCurrentImage();
	void endMainFrame();
};
