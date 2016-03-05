#include <vulkan/vulkan.h>
#include "vulkanyabause.h"

extern "C" {
#include "../config.h"
#include "yabause.h"
#include "scsp.h"
#include "vidvulkan.h"
#include "peripheral.h"
#include "m68kcore.h"
#include "sh2core.h"
#include "sh2int.h"
#include "cdbase.h"
#include "cs2.h"
#include "debug.h"
#include "sndal.h"
#include "osdcore.h"


	static char biospath[256] = "C:/ext/osusume/bios.bin";
	static char cdpath[256] = "C:/ext/osusume/VF1.img";
	static char buppath[256] = "./back.bin";
	static char mpegpath[256] = "\0";
	static char cartpath[256] = "\0";

	M68K_struct * M68KCoreList[] = {
		&M68KDummy,
#ifdef HAVE_C68K
		&M68KC68K,
#endif
#ifdef HAVE_Q68
		&M68KQ68,
#endif
		NULL
	};

	SH2Interface_struct *SH2CoreList[] = {
		&SH2Interpreter,
		&SH2DebugInterpreter,
		NULL
	};

	PerInterface_struct *PERCoreList[] = {
		&PERDummy,
		NULL
	};

	CDInterface *CDCoreList[] = {
		&DummyCD,
		&ISOCD,
		NULL
	};

	SoundInterface_struct *SNDCoreList[] = {
		&SNDDummy,
		NULL
	};

	VideoInterface_struct *VIDCoreList[] = {
		&VIDDummy,
		&viVIDVulkan,
	};

	OSD_struct *OSDCoreList[] = {
		&OSDDummy,
		NULL
	};

}

#define VERTEX_BUFFER_BIND_ID 0
// Note : 
//	Enabling this define will feed GLSL directly to the driver
//	Unlike the SDK samples that convert it to SPIR-V
//	This may or may not be supported depending on your ISV
//#define USE_GLSL
// Set to "true" to enable Vulkan's validation layers
// See vulkandebug.cpp for details
#define ENABLE_VALIDATION false



VulkanYabause::VulkanYabause() : VulkanBase(ENABLE_VALIDATION)
{
		width = 1280;
		height = 720;
		title = "Yabause Vulkan";
		// Values not set here are initialized in the base class constructor
}

VulkanYabause::~VulkanYabause()
{

}

int VulkanYabause::yabauseinit()
{
	int res;
	yabauseinit_struct yinit;
	void * padbits;

	yinit.m68kcoretype = M68KCORE_C68K;
	yinit.percoretype = PERCORE_DUMMY;
	yinit.sh2coretype = 0;
	yinit.vidcoretype = VIDCORE_VULKAN;
	yinit.sndcoretype = SNDCORE_DUMMY;
	yinit.cdcoretype = CDCORE_ISO;
	yinit.carttype = CART_NONE;
	yinit.regionid = 0;
	yinit.biospath = biospath;
	yinit.cdpath = cdpath;
	yinit.buppath = buppath;
	yinit.mpegpath = mpegpath;
	yinit.cartpath = cartpath;
	yinit.videoformattype = VIDEOFORMATTYPE_NTSC;
	yinit.frameskip = 0;
	yinit.usethreads = 1;
	yinit.skip_load = 0;
	res = YabauseInit(&yinit);
	if (res == -1)
	{
		return -1;
	}
	PerPortReset();
	padbits = PerPadAdd(&PORTDATA1);
#if 0
	PerSetKey(VK_KEY_UP, PERPAD_UP, padbits);
	PerSetKey(GLFW_KEY_RIGHT, PERPAD_RIGHT, padbits);
	PerSetKey(GLFW_KEY_DOWN, PERPAD_DOWN, padbits);
	PerSetKey(GLFW_KEY_LEFT, PERPAD_LEFT, padbits);
	PerSetKey(GLFW_KEY_Q, PERPAD_RIGHT_TRIGGER, padbits);
	PerSetKey(GLFW_KEY_E, PERPAD_LEFT_TRIGGER, padbits);
	PerSetKey(GLFW_KEY_ENTER, PERPAD_START, padbits);
	PerSetKey(GLFW_KEY_Z, PERPAD_A, padbits);
	PerSetKey(GLFW_KEY_X, PERPAD_B, padbits);
	PerSetKey(GLFW_KEY_C, PERPAD_C, padbits);
	PerSetKey(GLFW_KEY_A, PERPAD_X, padbits);
	PerSetKey(GLFW_KEY_S, PERPAD_Y, padbits);
	PerSetKey(GLFW_KEY_D, PERPAD_Z, padbits);
	g_Keymap[PERPAD_UP] = GLFW_KEY_UP;
	g_Keymap[PERPAD_RIGHT] = GLFW_KEY_RIGHT;
	g_Keymap[PERPAD_DOWN] = GLFW_KEY_DOWN;
	g_Keymap[PERPAD_LEFT] = GLFW_KEY_LEFT;
	g_Keymap[PERPAD_START] = GLFW_KEY_ENTER;
#endif
	ScspSetFrameAccurate(1);
	return 0;
}


void VulkanYabause::draw()
{
	YabauseExec();
}


void VulkanYabause::prepare()
{
	VulkanBase::prepare();

	// We need to tell the API the number of max. requested descriptors per type
	VkDescriptorPoolSize typeCounts[1];
	// This example only uses one descriptor type (uniform buffer) and only
	// requests one descriptor of this type
	typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	typeCounts[0].descriptorCount = 1;
	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// typeCounts[1].descriptorCount = 2;

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.pNext = NULL;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = typeCounts;
	// Set the max. number of sets that can be requested
	// Requesting descriptors beyond maxSets will result in an error
	descriptorPoolInfo.maxSets = 1;

	VkResult vkRes = vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool);
	assert(!vkRes);

	yabauseinit();
	prepared = true;
}

void VulkanYabause::render()
{
	if (!prepared)
		return;
	vkDeviceWaitIdle(device);
	draw();
	vkDeviceWaitIdle(device);
}

void VulkanYabause::viewChanged()
{

}

VkSemaphore VulkanYabause::beginMainFrame() {
	VkResult err;
	VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo = {};
	presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	presentCompleteSemaphoreCreateInfo.pNext = NULL;
	presentCompleteSemaphoreCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


	err = vkCreateSemaphore(device, &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);
	assert(!err);

	// Get next image in the swap chain (back/front buffer)
	err = swapChain.acquireNextImage(presentCompleteSemaphore, &currentBuffer);
	assert(!err);

	return presentCompleteSemaphore;
}

void VulkanYabause::endMainFrame() {
	VkResult err;
	// Present the current buffer to the swap chain
	// This will display the image
	err = swapChain.queuePresent(queue, currentBuffer);
	assert(!err);

	vkDestroySemaphore(device, presentCompleteSemaphore, nullptr);

	// Add a post present image memory barrier
	// This will transform the frame buffer color attachment back
	// to it's initial layout after it has been presented to the
	// windowing system
	// See buildCommandBuffers for the pre present barrier that 
	// does the opposite transformation 
	VkImageMemoryBarrier postPresentBarrier = {};
	postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	postPresentBarrier.pNext = NULL;
	postPresentBarrier.srcAccessMask = 0;
	postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	postPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	postPresentBarrier.image = swapChain.buffers[currentBuffer].image;

	// Use dedicated command buffer from example base class for submitting the post present barrier
	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	err = vkBeginCommandBuffer(postPresentCmdBuffer, &cmdBufInfo);
	assert(!err);

	// Put post present barrier into command buffer
	vkCmdPipelineBarrier(
		postPresentCmdBuffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_FLAGS_NONE,
		0, nullptr,
		0, nullptr,
		1, &postPresentBarrier);

	err = vkEndCommandBuffer(postPresentCmdBuffer);
	assert(!err);

	// Submit to the queue
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &postPresentCmdBuffer;

	err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	assert(!err);

	err = vkQueueWaitIdle(queue);
	assert(!err);


}


VkRenderPass VulkanYabause::getRenderPass() {
	return renderPass;
}

VkFramebuffer VulkanYabause::getCurrentFrameBuffer() {
	return frameBuffers[currentBuffer];
}

VkImage VulkanYabause::getCurrentImage() {
	return this->swapChain.buffers[currentBuffer].image;
}

int VulkanYabause::getBasicObjects(VkPhysicalDevice & physicalDevice, VkDevice & device, VkQueue & queue, VkCommandPool & cmdPool) {
	physicalDevice = this->physicalDevice;
	device = this->device;
	queue = this->queue;
	cmdPool = this->cmdPool;
	return 0;
}