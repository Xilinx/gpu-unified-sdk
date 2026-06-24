/*
 * Main loop driver.
 *
 * Implements the acquire -> render -> present loop, FPS reporting and
 * frame-count limiting. The --wayland / --display flags select the windowing
 * backend at runtime.
 */

#include "application.hpp"
#include "common.hpp"
#include "platform/platform.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace std;
using namespace VersalGPUSDK;

namespace
{
double getCurrentTime()
{
	timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
	{
		LOGE("clock_gettime() failed.\n");
		return 0.0;
	}
	return ts.tv_sec + ts.tv_nsec * 1e-9;
}
}

int main(int argc, char **argv)
{
	// Parse backend selection and optional frame count. A bare number sets
	// maxFrameCount.
	unsigned maxFrameCount = 0;
	bool useMaxFrameCount = false;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--wayland") == 0)
			platformSetBackend("wayland");
		else if (strcmp(argv[i], "--display") == 0)
			platformSetBackend("display");
		else if (strcmp(argv[i], "--maxframes") == 0 && i + 1 < argc)
		{
			maxFrameCount = strtoul(argv[++i], nullptr, 0);
			useMaxFrameCount = true;
		}
		else
		{
			// Backwards-compatible bare frame count.
			char *end = nullptr;
			unsigned long val = strtoul(argv[i], &end, 0);
			if (end && *end == '\0')
			{
				maxFrameCount = val;
				useMaxFrameCount = true;
			}
		}
	}

	Platform &platform = Platform::get();
	if (FAILED(platform.initialize()))
	{
		LOGE("Failed to initialize platform.\n");
		// Exit immediately without running static destructors: tearing down a
		// half-initialized Vulkan instance trips an unordered_map::at in some
		// Mali display drivers. The real failure has already been reported.
		_exit(1);
	}

	Platform::SwapchainDimensions dim = platform.getPreferredSwapchain();
	if (FAILED(platform.createWindow(dim)))
	{
		LOGE("Failed to create platform window.\n");
		_exit(1);
	}

	VulkanApplication *app = createApplication();
	if (!app)
	{
		LOGE("Failed to create application.\n");
		return 1;
	}

	if (!app->initialize(&platform.getContext()))
	{
		LOGE("Failed to initialize application.\n");
		return 1;
	}

	vector<VkImage> images;
	platform.getCurrentSwapchain(&images, &dim);
	app->updateSwapchain(images, dim);

	unsigned frameCount = 0;
	double startTime = getCurrentTime();

	while (platform.getWindowStatus() == Platform::STATUS_RUNNING)
	{
		unsigned index;
		Result res = platform.acquireNextImage(&index);
		while (res == RESULT_ERROR_OUTDATED_SWAPCHAIN)
		{
			res = platform.acquireNextImage(&index);
			platform.getCurrentSwapchain(&images, &dim);
			app->updateSwapchain(images, dim);
		}

		if (FAILED(res))
		{
			LOGE("Unrecoverable swapchain error.\n");
			break;
		}

		app->render(index, 0.0166f);
		res = platform.presentImage(index);

		// Handle Outdated error in acquire.
		if (FAILED(res) && res != RESULT_ERROR_OUTDATED_SWAPCHAIN)
			break;

		frameCount++;
		if (frameCount == 100)
		{
			double endTime = getCurrentTime();
			LOGI("FPS: %.3f\n", frameCount / (endTime - startTime));
			frameCount = 0;
			startTime = endTime;
		}

		if (useMaxFrameCount && (--maxFrameCount == 0))
			break;
	}

	app->terminate();
	delete app;
	platform.terminate();
}
