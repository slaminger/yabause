/*
* Vulkan Example - Basic indexed triangle rendering
*
* Note :
*	This is a "pedal to the metal" example to show off how to get Vulkan up an displaying something
*	Contrary to the other examples, this one won't make use of helper functions or initializers
*	Except in a few cases (swap chain setup e.g.)
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include "vulkanbase.h"
#include "vulkanyabause.h"


VulkanYabause *vulkanYabause;

extern "C" {
	int YuiRevokeOGLOnThisThread() {
		return 0;
	}

	int YuiUseOGLOnThisThread() {
		return 0;
	}

	void YuiErrorMsg(const char *string)
	{
		printf("%s", string);
	}

	void YuiSwapBuffers(void)
	{

	}
}


#ifdef _WIN32

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (vulkanYabause != NULL)
	{
		vulkanYabause->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

#else 

static void handleEvent(const xcb_generic_event_t *event)
{
	if (vulkanExample != NULL)
	{
		vulkanExample->handleEvent(event);
	}
}
#endif

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#else
int main(const int argc, const char *argv[])
#endif
{
	vulkanYabause = new VulkanYabause();
#ifdef _WIN32
	vulkanYabause->setupWindow(hInstance, WndProc);
#else
	vulkanExample->setupWindow();
#endif
	vulkanYabause->initSwapchain();
	vulkanYabause->prepare();
	vulkanYabause->renderLoop();
	delete(vulkanYabause);
	return 0;
}
