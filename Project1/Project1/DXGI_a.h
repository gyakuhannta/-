#pragma once
#include<d3d12.h>
#include<dxgi1_4.h>
#include <wrl/client.h>
class DXGI
{
public:
	DXGI() = default;
	~DXGI() = default;
	[[nodiscard]] bool setDisplayAdapter() noexcept;


	[[nodiscard]] IDXGIFactory4* factory() const noexcept;


	[[nodiscard]] IDXGIAdapter1* displayAdapter() const noexcept;

private:
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory_{};  /// DXGIを作成するファクトリー
	Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter_{};  /// ディスプレイモード取得用アダプタ
};


