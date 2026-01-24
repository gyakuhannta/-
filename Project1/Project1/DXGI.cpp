
#include<windows.h>
#include "DXGI_a.h"
#include <cassert>





IDXGIAdapter* GetHardwareAdapter(IDXGIFactory4* factory)
{
	 IDXGIAdapter1* adapter;

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
		OutputDebugStringW(L"Failed to create DXGI Factory\n");
		return nullptr;
	}
	return factory;

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
		if (FAILED(hr))
		{
			OutputDebugStringW(L"Failed to create D3D12 Device\n");
			return nullptr;
		}
		OutputDebugStringW(L"Using software adapter (WARP)\n");

	}
	return device;

}

ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device)
{

	D3D12_COMMAND_QUEUE_DESC queueDesc = { };
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	ID3D12CommandQueue* commandQueue;
	HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed to create Command Queue\n");
		return nullptr;
	}

	return commandQueue;
}
IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd)
{
	// スワップチェーンの詳細設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 2;                      // ダブルバッファリング
	swapChainDesc.Width = 1280;                         // 画面幅
	swapChainDesc.Height = 720;                         // 画面高
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ピクセルフォーマット
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // レンダーターゲット用
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // 高速切替
	swapChainDesc.SampleDesc.Count = 1;                 // マルチサンプリングなし

	IDXGISwapChain1* swapChain1;
	HRESULT hr = factory->CreateSwapChainForHwnd(
		commandQueue,       // コマンドキュー
		hwnd,              // ターゲットウィンドウ
		&swapChainDesc,    // 設定
		nullptr,           // フルスクリーン設定
		nullptr,           // 出力制限
		&swapChain1        // 作成されるスワップチェーン
	);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed to create Swap Chain\n");
		return nullptr;
	}

	// より高機能なインターフェースにキャスト
	IDXGISwapChain3* swapChain;
	hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain));
	swapChain1->Release();

	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed to cast to SwapChain3\n");
		return nullptr;
	}

	return swapChain;
}
void EnableDebugLayer()
{
#if defined(_DEBUG)
	// デバッグインターフェースを取得
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// デバッグレイヤーを有効化
		debugController->EnableDebugLayer();

		// より詳細な検証を有効化（任意）
		ID3D12Debug1* debugController1;
		if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1))))
		{
			debugController1->SetEnableGPUBasedValidation(TRUE);
		}
	}
#endif
}