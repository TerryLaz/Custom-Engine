// ELEFTERIOS LAZARIDIS
#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(static_cast<unsigned int>(time(nullptr)));// Initialize random seed

	HRESULT hr = CoInitialize(nullptr);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to call CoInitialize", L"Error message", MB_OK | MB_ICONINFORMATION);
		return -1;
	}

	Engine engine;
	if (engine.Initialize(hInstance, 1600, 900))
	{
		while (engine.ProcessMessages())
		{
			engine.Update();
			engine.Render();
		}
	}

	return 0;
}