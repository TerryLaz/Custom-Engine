// ELEFTERIOS LAZARIDIS
#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

#include "Keyboard.h"
#include "Mouse.h"

class WindowManager
{
private:
	HWND hWnd = nullptr;
	HINSTANCE hInstance = nullptr;
	int width = 0;
	int height = 0;
public:
	Keyboard keyboard;
	Mouse mouse;

	WindowManager();
	~WindowManager();

	bool Initialize(HINSTANCE hInstance, int width, int height);
	bool ProcessMessages();

	HWND GetHandle() const;
	HINSTANCE GetIntance() const;
	int GetWidth() const;
	int GetHeight() const;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgRedirect(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
