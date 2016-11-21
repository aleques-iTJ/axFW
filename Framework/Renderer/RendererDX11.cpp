#include "Renderer/Renderer.h"
#include "Renderer/Device.h"

#include "Core/Utility/UTFConversion.h"
//#include "Core/PF_MinWin.h"

#undef __midl
#include <dxgi.h>
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")




Array<DeviceID> Renderer::EnumerateDevices()
{
	IDXGIFactory1* dxgiFactory = nullptr;
	CreateDXGIFactory1(IID_IDXGIFactory1, reinterpret_cast<void**>(&dxgiFactory));

	Array<Device>	devices;
	IDXGIAdapter1*	adapter = nullptr;


	for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		Device device;
		device.Name			= "Test";
		device.TotalMemory	= desc.DedicatedVideoMemory;

		//memcpy(device.pfData, &dxgiFactory, sizeof(IDXGIFactory1*));
		//memcpy(device.pfData + sizeof(IDXGIFactory1*), &adapter, sizeof(IDXGIAdapter1*));

		devices.PushBack(device);
	}

	return devices;
}