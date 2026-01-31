
#include<windows.h>
#include "DXGI_a.h"
#include <cassert>
#include<d3d12.h>
#include<dxgi1_6.h>






IDXGIFactory4* CreateDXGIFactory()//DXGIファクトリーの作成
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
IDXGIAdapter* GetHardwareAdapter(IDXGIFactory4* factory)//ハードウェアアダプタの取得
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

		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			return adapter;
		}

		adapter->Release();

	}

	return nullptr;


}

ID3D12Device* CreateD3D12Device(IDXGIAdapter1* adapter)//デバイスの作成
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

ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device)//コマンドキューの作成
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
IDXGISwapChain3* CreateSwapChain
(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd)//スワップチェーンの作成
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
void EnableDebugLayer()//デバッグレイヤーの有効化
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
ID3D12DescriptorHeap* CreateRTVHeap(ID3D12Device* device)//RTVヒープの作成
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ID3D12DescriptorHeap* rtvHeap = nullptr;;
	HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	if (FAILED(hr))
	{
		return nullptr;
	}
	return rtvHeap;
}

ID3D12CommandAllocator* CreateCommandAllocator(ID3D12Device* device)//コマンドアロケータの作成
{

	ID3D12CommandAllocator* commandAllocator = nullptr;//宣言と初期化
	HRESULT hr = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator)
	);//メモリ領域の作成
	if (FAILED(hr))//エラー判定
	{
		OutputDebugStringW(L"Failed to create Command Allocator\n");
		return nullptr;
	}
	return commandAllocator;
}

ID3D12GraphicsCommandList* CreateCommandList
(ID3D12Device* device, ID3D12CommandAllocator* commandAllocator)//コマンドリストの作成
{
	ID3D12GraphicsCommandList* commandList = nullptr;
	HRESULT hr = device->CreateCommandList(
		0,                                // マルチGPUではない場合は0
		D3D12_COMMAND_LIST_TYPE_DIRECT,   // アロケータと同じタイプを指定
		commandAllocator,                 // 紐付けるメモ帳（アロケータ）
		nullptr,                          // 最初に使用するパイプライン状態（後ほど設定）
		IID_PPV_ARGS(&commandList)        // 作成されたリストの格納先
	);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed to create Command List\n");
		return nullptr;
	}
	return commandList;
}
ID3D12Fence* CreateFence(ID3D12Device* device) // フェンスの作成
{
	ID3D12Fence* fence = nullptr;
	// 初期値を0としてフェンスを作成
	HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed to create Fence\n");
		return nullptr;
	}
	return fence;
}