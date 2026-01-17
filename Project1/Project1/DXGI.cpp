#include "DXGI_a.h"
#include <cassert>










IDXGIFactory4* CreateDXGIFactory()
{
	IDXGIFactory4* factory;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // defined(_DEBUG)
	HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory));

	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create DXGI Factory\n");
		return nullptr;
	}
	return factory;

}


IDXGIAdapter* GetHardwareAdapter(IDXGIFactory4* factory)
{
	 IDXGIAdapter* adapter;

	for (UINT adaputerIndex = 0; ; ++adaputerIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adaputerIndex, &adapter))
		{
			break;
		}

		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			adapter->Release();
			continue;

		}

		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device),nullptr)))
		{
			return adapter;
		}

		adapter->Release();

	}

	return nullptr;


}
ID3D12Device* CreateD3D12Device(IDXGIAdapter1* adapter)
{
	ID3D12Device* device;

	HRESULT hr = D3D12CreateDevice(
		adapter,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device)
	);

	if (FAILED(hr))
	{
		hr = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device)
		);


	}
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create D3D12 Device\n");
		return nullptr;
	}

}