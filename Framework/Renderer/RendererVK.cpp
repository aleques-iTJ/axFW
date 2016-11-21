#include "Renderer/Renderer.h"
#include "Renderer/Device.h"

#include "Core/Utility/UTFConversion.h"
#include "Core/PF_MinWin.h"

#include <vulkan/vulkan.h>
//#pragma comment (lib, "vulkan-1.lib")



/*Array<Device> Renderer::EnumerateDevices()
{
	VkApplicationInfo appInfo;
	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName	= "bnrFW";
	appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName			= "bnrFW Vulkan";
	appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion			= VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo;
	memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));
	createInfo.sType			= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	VkResult result;

	VkInstance instance;
	result = vkCreateInstance(&createInfo, nullptr, &instance);

	
	uint32 deviceCount = 0;
	result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);	// Get the device count first

	if (result != VK_SUCCESS || result != VK_INCOMPLETE)
	{
		// No valid Vulkan devices...
		return Array<Device>();
	}

	else if (deviceCount > 0)
	{
		// There's at least 1 valid device at this point
		VkPhysicalDevice devices[16] = { 0 };
		
		result = vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[0], &deviceProperties);

		VkQueueFamilyProperties queueFamilyProperties[16]	= { 0 };
		uint32					queueFamilyCount			= 0;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &queueFamilyCount, nullptr);
		vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &queueFamilyCount, queueFamilyProperties);

		VkDeviceQueueCreateInfo queueCreateInfo = { };
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = //queueFamilyProperties[0].;
		queueCreateInfo.queueCount = 1;
	}	

	return Array<Device>();
}*/