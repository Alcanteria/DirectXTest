#pragma once

#include <Windows.h>
#include <string>
#include "Common.h"
#include "GameClock.h"
#include "GameTime.h"
#include "GameComponent.h"

using namespace Library;

namespace Library
{
	class Game
	{
	public:
		Game(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
		~Game();
		HINSTANCE Instance() const;
		HWND WindowHandle() const;
		const WNDCLASSEX& Window() const;
		const std::wstring& WindowClass() const;
		const std::wstring& WindowTitle() const;
		int ScreenWidth() const;
		int ScreenHeight() const;
		float AspectRatio() const;
		virtual void Run();
		virtual void Exit();
		virtual void Initialize();
		virtual void Update(const GameTime& gameTime);
		virtual void Draw(const GameTime& gameTime);

		ID3D11Device1* Direct3DDevice() const;
		ID3D11DeviceContext1* Direct3DDeviceContext() const;
		bool DepthBufferEnabled() const;
		bool IsFullScreen() const;
		const D3D11_TEXTURE2D_DESC& BackBufferDesc() const;
		const D3D11_VIEWPORT& Viewport() const;

		const std::vector<GameComponent*>& Components() const;

	protected:
		virtual void InitializeWindow();
		virtual void Shutdown();
		static const UINT DefaultScreenWidth = 1024;
		static const UINT DefaultScreenHeight = 768;
		HINSTANCE mInstance;
		std::wstring mWindowClass;
		std::wstring mWindowTitle;
		int mShowCommand;
		HWND mWindowHandle;
		WNDCLASSEX mWindow;
		UINT mScreenWidth;
		UINT mScreenHeight;
		GameClock mGameClock;
		GameTime mGameTime;

		virtual void InitializeDirectX();

		static const UINT DefaultFrameRate;
		static const UINT DefaultMultiSamplingCount;

		D3D_FEATURE_LEVEL mFeatureLevel;
		ID3D11Device1* mDirect3DDevice;
		ID3D11DeviceContext1* mDirect3DDeviceContext;
		IDXGISwapChain1* mSwapChain;

		UINT mFrameRate;
		bool mIsFullScreen;
		bool mDepthStencilBufferEnabled;
		bool mMultisamplingEnabled;
		UINT mMultisamplingCount;
		UINT mMultisamplingQualityLevels;

		ID3D11Texture2D* mDepthStencilBuffer;
		D3D11_TEXTURE2D_DESC mBackBufferDesc;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView* mDepthStencilView;
		D3D11_VIEWPORT mViewport;

		std::vector<GameComponent*> mComponents;

	private:
		Game(const Game& rhs);
		Game& operator=(const Game& rhs);
		POINT CenterWindow(int windowWidth, int windowHeight);
		static LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	};
}