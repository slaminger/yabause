extern "C" {
#include "vidvulkan.h"
#include "vidshared.h"
#include "debug.h"
#include "vdp2.h"
#include "yabause.h"
#include "ygl.h"
#include "yui.h"
#include "frameprofile.h"
}

#include "VulkanYabause.h"
extern VulkanYabause *vulkanYabause;

#define VERTEX_BUFFER_BIND_ID 0

class VIDVulkan {

protected:
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
	VkCommandPool cmdPool;
	VkRenderPass renderPass;
	VkCommandBuffer drawCmdBuffer;
	VkPipelineLayout pipelineLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;


	// List of shader modules created (stored for cleanup)
	std::vector<VkShaderModule> shaderModules;
	// Pipeline cache object
	VkPipelineCache pipelineCache;

public:
	struct {
		VkBuffer buf;
		VkDeviceMemory mem;
		VkPipelineVertexInputStateCreateInfo vi;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		VkPipeline solid;
	} pipelines;

	struct {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo descriptor;
	}  uniformDataVS;

	struct {
		int count;
		VkBuffer buf;
		VkDeviceMemory mem;
	} indices;

	struct {
		glm::mat4 u_mvpMatrix;
	} uboVS;

public:
	VIDVulkan();
	~VIDVulkan();
	int Init(void);
	void DeInit(void);
	void Resize(unsigned int, unsigned int, int);
	int IsFullscreen(void);
	int Vdp1Reset(void);
	void Vdp1DrawStart(void);
	void Vdp1DrawEnd(void);
	void Vdp1NormalSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer);
	void Vdp1ScaledSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer);
	void Vdp1DistortedSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer);
	void Vdp1PolygonDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer);
	void Vdp1PolylineDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer);
	void Vdp1LineDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer);
	void Vdp1UserClipping(u8 * ram, Vdp1 * regs);
	void Vdp1SystemClipping(u8 * ram, Vdp1 * regs);
	void Vdp1LocalCoordinate(u8 * ram, Vdp1 * regs);
	int  Vdp2Reset(void);
	void Vdp2DrawStart(void);
	void Vdp2DrawEnd(void);
	void Vdp2DrawScreens(void);
	void Vdp2SetResolution(u16 TVMD);
	void GetGlSize(int *width, int *height);
	void Vdp1ReadFrameBuffer(u32 type, u32 addr, void * out);
	void SetFilterMode(int type);
	void Sync();

protected:
	VkPipelineShaderStageCreateInfo loadShader(const char * fileName, VkShaderStageFlagBits stage);
	void prepareVertices();
};

VIDVulkan * vidVulkan = nullptr;

extern "C" {

	int VIDVulkanInit(void) {
		if(vidVulkan==nullptr)
			vidVulkan = new VIDVulkan();
		return vidVulkan->Init();
	}
	void VIDVulkanDeInit(void) { vidVulkan->DeInit(); }
	void VIDVulkanResize(unsigned int w, unsigned int h, int d ) { vidVulkan->Resize(w, h, d); }
	int VIDVulkanIsFullscreen(void) { return vidVulkan->IsFullscreen();	}
	int VIDVulkanVdp1Reset(void) { return vidVulkan->Vdp1Reset();}
	void VIDVulkanVdp1DrawStart(void) {	vidVulkan->Vdp1DrawStart();	}
	void VIDVulkanVdp1DrawEnd(void) { vidVulkan->Vdp1DrawEnd();	}
	void VIDVulkanVdp1NormalSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) { vidVulkan->Vdp1NormalSpriteDraw(ram, regs, back_framebuffer);	}
	void VIDVulkanVdp1ScaledSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) { vidVulkan->Vdp1ScaledSpriteDraw(ram, regs, back_framebuffer);	}
	void VIDVulkanVdp1DistortedSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) { vidVulkan->Vdp1DistortedSpriteDraw(ram, regs, back_framebuffer); }
	void VIDVulkanVdp1PolygonDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) { vidVulkan->Vdp1PolygonDraw(ram, regs, back_framebuffer); }
	void VIDVulkanVdp1PolylineDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) { vidVulkan->Vdp1PolylineDraw(ram, regs, back_framebuffer);	}
	void VIDVulkanVdp1LineDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) { vidVulkan->Vdp1LineDraw(ram, regs, back_framebuffer);	}
	void VIDVulkanVdp1UserClipping(u8 * ram, Vdp1 * regs) { vidVulkan->Vdp1UserClipping(ram, regs);	}
	void VIDVulkanVdp1SystemClipping(u8 * ram, Vdp1 * regs) { vidVulkan->Vdp1SystemClipping(ram, regs);	}
	void VIDVulkanVdp1LocalCoordinate(u8 * ram, Vdp1 * regs) { vidVulkan->Vdp1LocalCoordinate(ram, regs);}
	int VIDVulkanVdp2Reset(void) { return vidVulkan->Vdp2Reset(); }
	void VIDVulkanVdp2DrawStart(void) { vidVulkan->Vdp2DrawStart();	}
	void VIDVulkanVdp2DrawEnd(void) { vidVulkan->Vdp2DrawEnd();	}
	void VIDVulkanVdp2DrawScreens(void) { vidVulkan->Vdp2DrawScreens();	}
	void VIDVulkanVdp2SetResolution(u16 TVMD) { vidVulkan->Vdp2SetResolution(TVMD);	}
	void YglGetGlSize(int *width, int *height) { vidVulkan->GetGlSize(width, height);}
	void VIDVulkanVdp1ReadFrameBuffer(u32 type, u32 addr, void * out) { vidVulkan->Vdp1ReadFrameBuffer(type,addr,out);	}
	void VIDVulkanSetFilterMode(int type) { 	vidVulkan->SetFilterMode(type);	}
	void VIDVulkanSync() {	vidVulkan->Sync();}

	VideoInterface_struct viVIDVulkan = {
		VIDCORE_VULKAN,
		"Vulkan Video Interface",
		VIDVulkanInit,
		VIDVulkanDeInit,
		VIDVulkanResize,
		VIDVulkanIsFullscreen,
		VIDVulkanVdp1Reset,
		VIDVulkanVdp1DrawStart,
		VIDVulkanVdp1DrawEnd,
		VIDVulkanVdp1NormalSpriteDraw,
		VIDVulkanVdp1ScaledSpriteDraw,
		VIDVulkanVdp1DistortedSpriteDraw,
		VIDVulkanVdp1PolygonDraw,
		VIDVulkanVdp1PolylineDraw,
		VIDVulkanVdp1LineDraw,
		VIDVulkanVdp1UserClipping,
		VIDVulkanVdp1SystemClipping,
		VIDVulkanVdp1LocalCoordinate,
		VIDVulkanVdp1ReadFrameBuffer,
		NULL,
		VIDVulkanVdp2Reset,
		VIDVulkanVdp2DrawStart,
		VIDVulkanVdp2DrawEnd,
		VIDVulkanVdp2DrawScreens,
		YglGetGlSize,
		VIDVulkanSetFilterMode,
		VIDVulkanSync
	};
}

VIDVulkan::VIDVulkan() {

}

VIDVulkan::~VIDVulkan() {

}

int VIDVulkan::Init(void) {

	VkResult err;
	VkResult vkRes;

	// Setup Objecs
	vulkanYabause->getBasicObjects(physicalDevice, device, queue, cmdPool);
	renderPass = vulkanYabause->getRenderPass();

	VkCommandBufferAllocateInfo cmdBufAllocateInfo =
		vkTools::initializers::commandBufferAllocateInfo(
			cmdPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			(uint32_t)1);

	vkRes = vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &drawCmdBuffer);
	assert(!vkRes);


	//-----------------------------------------------------------------------------------
	// Vertex shader uniform buffer block
	VkBufferCreateInfo bufferInfo = {};
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.allocationSize = 0;
	allocInfo.memoryTypeIndex = 0;
	VkMemoryRequirements memReqs;

	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(uboVS);
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	// Create a new buffer
	err = vkCreateBuffer(device, &bufferInfo, nullptr, &uniformDataVS.buffer);
	assert(!err);
	// Get memory requirements including size, alignment and memory type 
	vkGetBufferMemoryRequirements(device, uniformDataVS.buffer, &memReqs);
	allocInfo.allocationSize = memReqs.size;
	// Gets the appropriate memory type for this type of buffer allocation
	// Only memory types that are visible to the host
	vulkanYabause->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &allocInfo.memoryTypeIndex);
	// Allocate memory for the uniform buffer
	err = vkAllocateMemory(device, &allocInfo, nullptr, &(uniformDataVS.memory));
	assert(!err);
	// Bind memory to buffer
	err = vkBindBufferMemory(device, uniformDataVS.buffer, uniformDataVS.memory, 0);
	assert(!err);

	// Store information in the uniform's descriptor
	uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
	uniformDataVS.descriptor.offset = 0;
	uniformDataVS.descriptor.range = sizeof(uboVS);

	uboVS.u_mvpMatrix = glm::ortho(0.0f, 320.0f, 224.0f, 0.0f, 10.0f, 0.0f);

	// Map uniform buffer and update it
	uint8_t *pData;
	err = vkMapMemory(device, uniformDataVS.memory, 0, sizeof(uboVS), 0, (void **)&pData);
	assert(!err);
	memcpy(pData, &uboVS, sizeof(uboVS));
	vkUnmapMemory(device, uniformDataVS.memory);
	assert(!err);

	// Setup layout of descriptors used in this example
	// Basically connects the different shader stages to descriptors
	// for binding uniform buffers, image samplers, etc.
	// So every shader binding should map to one descriptor set layout
	// binding

	// Binding 0 : Uniform buffer (Vertex shader)
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
	descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorLayout.pNext = NULL;
	descriptorLayout.bindingCount = 1;
	descriptorLayout.pBindings = &layoutBinding;

	err = vkCreateDescriptorSetLayout(device, &descriptorLayout, NULL, &descriptorSetLayout);
	assert(!err);

	// Create the pipeline layout that is used to generate the rendering pipelines that
	// are based on this descriptor set layout
	// In a more complex scenario you would have different pipeline layouts for different
	// descriptor set layouts that could be reused
	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pPipelineLayoutCreateInfo.pNext = NULL;
	pPipelineLayoutCreateInfo.setLayoutCount = 1;
	pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	assert(!err);

	// Setup Vdp2 pipeline
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};

	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	// The layout used for this pipeline
	pipelineCreateInfo.layout = pipelineLayout;
	// Renderpass this pipeline is attached to
	pipelineCreateInfo.renderPass = renderPass;

	// Vertex input state
	// Describes the topoloy used with this pipeline
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
	inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	// This pipeline renders vertex data as triangle lists
	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	// Rasterization state
	VkPipelineRasterizationStateCreateInfo rasterizationState = {};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	// Solid polygon mode
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	// No culling
	rasterizationState.cullMode = VK_CULL_MODE_NONE;
	rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.rasterizerDiscardEnable = VK_FALSE;
	rasterizationState.depthBiasEnable = VK_FALSE;

	// Color blend state
	// Describes blend modes and color masks
	VkPipelineColorBlendStateCreateInfo colorBlendState = {};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	// One blend attachment state
	// Blending is not used in this example
	VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
	blendAttachmentState[0].colorWriteMask = 0xf;
	blendAttachmentState[0].blendEnable = VK_FALSE;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = blendAttachmentState;

	// Viewport state
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	// One viewport
	viewportState.viewportCount = 1;
	// One scissor rectangle
	viewportState.scissorCount = 1;

	// Enable dynamic states
	// Describes the dynamic states to be used with this pipeline
	// Dynamic states can be set even after the pipeline has been created
	// So there is no need to create new pipelines just for changing
	// a viewport's dimensions or a scissor box
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	// The dynamic state properties themselves are stored in the command buffer
	std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables.data();
	dynamicState.dynamicStateCount = dynamicStateEnables.size();

	// Depth and stencil state
	// Describes depth and stenctil test and compare ops
	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	// Basic depth compare setup with depth writes and depth test enabled
	// No stencil used 
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = VK_TRUE;
	depthStencilState.depthWriteEnable = VK_TRUE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilState.depthBoundsTestEnable = VK_FALSE;
	depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
	depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
	depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depthStencilState.stencilTestEnable = VK_FALSE;
	depthStencilState.front = depthStencilState.back;

	// Multi sampling state
	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.pSampleMask = NULL;
	// No multi sampling used in this example
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Load shaders
	VkPipelineShaderStageCreateInfo shaderStages[2] = { {},{} };
	shaderStages[0] = loadShader("./../data/shaders/normal.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	shaderStages[1] = loadShader("./../data/shaders/normal.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	prepareVertices();

	// Assign states
	// Two shader stages
	pipelineCreateInfo.stageCount = 2;
	// Assign pipeline state create information
	pipelineCreateInfo.pVertexInputState = &vertices.vi;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pMultisampleState = &multisampleState;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pStages = shaderStages;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.pDynamicState = &dynamicState;

	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	err = vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
	assert(!err);

	// Create rendering pipeline
	err = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines.solid);
	assert(!err);


	//------------------------------------------------------------------------------------
	
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

	vkRes = vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool);
	assert(!vkRes);

	// Update descriptor sets determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point
	VkWriteDescriptorSet writeDescriptorSet = {};

	VkDescriptorSetAllocateInfo dsallocInfo = {};
	dsallocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	dsallocInfo.descriptorPool = descriptorPool;
	dsallocInfo.descriptorSetCount = 1;
	dsallocInfo.pSetLayouts = &descriptorSetLayout;

	vkRes = vkAllocateDescriptorSets(device, &dsallocInfo, &descriptorSet);
	assert(!vkRes);

	// Binding 0 : Uniform buffer
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = descriptorSet;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &uniformDataVS.descriptor;
	// Binds this uniform buffer to binding point 0
	writeDescriptorSet.dstBinding = 0;

	vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, NULL);


	return 0;
}

VkPipelineShaderStageCreateInfo VIDVulkan::loadShader(const char * fileName, VkShaderStageFlagBits stage)
{
	VkPipelineShaderStageCreateInfo shaderStage = {};
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = stage;
	shaderStage.module = vkTools::loadShader(fileName, device, stage);
	shaderStage.pName = "main"; // todo : make param
	assert(shaderStage.module != NULL);
	shaderModules.push_back(shaderStage.module);
	return shaderStage;
}

// Setups vertex and index buffers for an indexed triangle,
// uploads them to the VRAM and sets binding points and attribute
// descriptions to match locations inside the shaders
void VIDVulkan::prepareVertices()
{
	struct Vertex {
		float pos[3];
		float col[3];
	};

	// Setup vertices
	std::vector<Vertex> vertexBuffer = {
		{ { 0.0f,  0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } },
		{ { 320.0f,  0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
		{ { 150.0f, 224.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
	};
	int vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);

	// Setup indices
	std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
	int indexBufferSize = indexBuffer.size() * sizeof(uint32_t);

	VkMemoryAllocateInfo memAlloc = {};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAlloc.pNext = NULL;
	memAlloc.allocationSize = 0;
	memAlloc.memoryTypeIndex = 0;
	VkMemoryRequirements memReqs;

	VkResult err;
	void *data;

	// Generate vertex buffer
	//	Setup
	VkBufferCreateInfo bufInfo = {};
	bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufInfo.pNext = NULL;
	bufInfo.size = vertexBufferSize;
	bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufInfo.flags = 0;
	//	Copy vertex data to VRAM
	memset(&vertices, 0, sizeof(vertices));
	err = vkCreateBuffer(device, &bufInfo, nullptr, &vertices.buf);
	assert(!err);
	vkGetBufferMemoryRequirements(device, vertices.buf, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	vulkanYabause->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);
	vkAllocateMemory(device, &memAlloc, nullptr, &vertices.mem);
	assert(!err);
	err = vkMapMemory(device, vertices.mem, 0, memAlloc.allocationSize, 0, &data);
	assert(!err);
	memcpy(data, vertexBuffer.data(), vertexBufferSize);
	vkUnmapMemory(device, vertices.mem);
	assert(!err);
	err = vkBindBufferMemory(device, vertices.buf, vertices.mem, 0);
	assert(!err);

	// Generate index buffer
	//	Setup
	VkBufferCreateInfo indexbufferInfo = {};
	indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	indexbufferInfo.pNext = NULL;
	indexbufferInfo.size = indexBufferSize;
	indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	indexbufferInfo.flags = 0;
	// Copy index data to VRAM
	memset(&indices, 0, sizeof(indices));
	err = vkCreateBuffer(device, &bufInfo, nullptr, &indices.buf);
	assert(!err);
	vkGetBufferMemoryRequirements(device, indices.buf, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	vulkanYabause->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);
	err = vkAllocateMemory(device, &memAlloc, nullptr, &indices.mem);
	assert(!err);
	err = vkMapMemory(device, indices.mem, 0, indexBufferSize, 0, &data);
	assert(!err);
	memcpy(data, indexBuffer.data(), indexBufferSize);
	vkUnmapMemory(device, indices.mem);
	err = vkBindBufferMemory(device, indices.buf, indices.mem, 0);
	assert(!err);
	indices.count = indexBuffer.size();

	// Binding description
	vertices.bindingDescriptions.resize(1);
	vertices.bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	vertices.bindingDescriptions[0].stride = sizeof(Vertex);
	vertices.bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// Attribute descriptions
	// Describes memory layout and shader attribute locations
	vertices.attributeDescriptions.resize(2);
	// Location 0 : Position
	vertices.attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	vertices.attributeDescriptions[0].location = 0;
	vertices.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertices.attributeDescriptions[0].offset = 0;
	vertices.attributeDescriptions[0].binding = 0;
	// Location 1 : Color
	vertices.attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
	vertices.attributeDescriptions[1].location = 1;
	vertices.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertices.attributeDescriptions[1].offset = sizeof(float) * 3;
	vertices.attributeDescriptions[1].binding = 0;

	// Assign to vertex buffer
	vertices.vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertices.vi.pNext = NULL;
	vertices.vi.vertexBindingDescriptionCount = vertices.bindingDescriptions.size();
	vertices.vi.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
	vertices.vi.vertexAttributeDescriptionCount = vertices.attributeDescriptions.size();
	vertices.vi.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
}


void VIDVulkan::DeInit(void) {

}

void VIDVulkan::Resize(unsigned int, unsigned int, int) {

}

int VIDVulkan::IsFullscreen(void) {
	return 0;
}

int VIDVulkan::Vdp1Reset(void) {
	return 0;
}

void VIDVulkan::Vdp1DrawStart(void) {

}

void VIDVulkan::Vdp1DrawEnd(void) {

}

void VIDVulkan::Vdp1NormalSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) {

}

void VIDVulkan::Vdp1ScaledSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) {

}

void VIDVulkan::Vdp1DistortedSpriteDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) {

}

void VIDVulkan::Vdp1PolygonDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) {

}

void VIDVulkan::Vdp1PolylineDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) {

}

void VIDVulkan::Vdp1LineDraw(u8 * ram, Vdp1 * regs, u8* back_framebuffer) {

}

void VIDVulkan::Vdp1UserClipping(u8 * ram, Vdp1 * regs) {

}

void VIDVulkan::Vdp1SystemClipping(u8 * ram, Vdp1 * regs) {

}

void VIDVulkan::Vdp1LocalCoordinate(u8 * ram, Vdp1 * regs) {

}

int  VIDVulkan::Vdp2Reset(void) {
	return 0;
}

void VIDVulkan::Vdp2DrawStart(void) {

}

void VIDVulkan::Vdp2DrawEnd(void) {
	VkSemaphore presentCompleteSemaphore;
	presentCompleteSemaphore = vulkanYabause->beginMainFrame();

	int width = 1280;
	int height = 720;


	VkResult err;
	VkClearColorValue defaultClearColor = { { 1.000f, 0.025f, 0.025f, 1.0f } };
	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufInfo.pNext = NULL;

	VkClearValue clearValues[2];
	clearValues[0].color = defaultClearColor;
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = NULL;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = width;
	renderPassBeginInfo.renderArea.extent.height = height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = vulkanYabause->getCurrentFrameBuffer();

	err = vkBeginCommandBuffer(drawCmdBuffer, &cmdBufInfo);
	assert(!err);
	vkCmdBeginRenderPass(drawCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Update dynamic viewport state
	VkViewport viewport = {};
	viewport.height = (float)height;
	viewport.width = (float)width;
	viewport.minDepth = (float) 0.0f;
	viewport.maxDepth = (float) 1.0f;
	vkCmdSetViewport(drawCmdBuffer, 0, 1, &viewport);

	// Update dynamic scissor state
	VkRect2D scissor = {};
	scissor.extent.width = width;
	scissor.extent.height = height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(drawCmdBuffer, 0, 1, &scissor);

	// Bind descriptor sets describing shader binding points
	vkCmdBindDescriptorSets(drawCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

	// Bind the rendering pipeline (including the shaders)
	vkCmdBindPipeline(drawCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.solid);

	// Bind triangle vertices
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(drawCmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &vertices.buf, offsets);

	// Bind triangle indices
	vkCmdBindIndexBuffer(drawCmdBuffer, indices.buf, 0, VK_INDEX_TYPE_UINT32);

	// Draw indexed triangle
	vkCmdDrawIndexed(drawCmdBuffer, indices.count, 1, 0, 0, 1);

	vkCmdEndRenderPass(drawCmdBuffer);

	// Add a present memory barrier to the end of the command buffer
	// This will transform the frame buffer color attachment to a
	// new layout for presenting it to the windowing system integration 
	VkImageMemoryBarrier prePresentBarrier = {};
	prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	prePresentBarrier.pNext = NULL;
	prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	prePresentBarrier.dstAccessMask = 0;
	prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	prePresentBarrier.image = vulkanYabause->getCurrentImage();

	VkImageMemoryBarrier *pMemoryBarrier = &prePresentBarrier;
	vkCmdPipelineBarrier(
		drawCmdBuffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_FLAGS_NONE,
		0, nullptr,
		0, nullptr,
		1, &prePresentBarrier);

	err = vkEndCommandBuffer(drawCmdBuffer);
	assert(!err);

	// The submit infor strcuture contains a list of
	// command buffers and semaphores to be submitted to a queue
	// If you want to submit multiple command buffers, pass an array
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &drawCmdBuffer;

	// Submit to the graphics queue
	err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	assert(!err);

	vulkanYabause->endMainFrame();

}

void VIDVulkan::Vdp2DrawScreens(void) {

}

void VIDVulkan::Vdp2SetResolution(u16 TVMD) {

}

void VIDVulkan::GetGlSize(int *width, int *height) {

}
void VIDVulkan::Vdp1ReadFrameBuffer(u32 type, u32 addr, void * out) {

}

void VIDVulkan::SetFilterMode(int type) {

}
void VIDVulkan::Sync() {

}
