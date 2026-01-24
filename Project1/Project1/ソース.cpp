#include <Windows.h>
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		//画面更新処理
		return 0;
	case WM_KEYDOWN:
		//キー入力処理
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);

}

int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE hPrevinstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hinstance;
	wc.lpszClassName = "GameWindow";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	RegisterClass(&wc);

	HWND hwnd = CreateWindow(
		"GameWindow",
		"My GAme",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL, NULL,
		hinstance,
		NULL
	);

	ShowWindow(hwnd, nCmdShow);

	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))//修正点
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	};
	
    


}