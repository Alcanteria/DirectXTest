#include "stdafx.h"
#include "RenderingGame.h"
#include <sstream>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "..\Library\GameException.h"
#include "..\Library\Keyboard.h"
#include "..\Library\Mouse.h"
#include "..\Library\FpsComponent.h"
#include "..\Library\Utility.h"
#include <iostream>

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = { 0.392f, 0.584f, 0.929f, 1.0f };

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand) 
		: Game(instance, windowClass, windowTitle, showCommand)
	{
		mDepthStencilBufferEnabled = true;
		mMultisamplingEnabled = true;
	}

	RenderingGame::~RenderingGame()
	{

	}

	void RenderingGame::Initialize()
	{
		if (FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mDirectInput, nullptr)))
		{
			throw GameException("DirectInput8Create() failed");
		}

		mKeyboard = new Keyboard(*this, mDirectInput);
		mComponents.push_back(mKeyboard);
		mServices.AddService(Keyboard::TypeIdClass(), mKeyboard);

		mMouse = new Mouse(*this, mDirectInput);
		mComponents.push_back(mMouse);
		mServices.AddService(Mouse::TypeIdClass(), mMouse);

		mFpsComponent = new FpsComponent(*this);
		mComponents.push_back(mFpsComponent);

		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
		std::cout << "Attempting to create sprite batch." << std::endl;
		mSpriteBatch = new SpriteBatch(mDirect3DDeviceContext);
		
		
		mSpriteFont = new SpriteFont(mDirect3DDevice, L"C:\\Users\\Nick\\Source\\Repos\\DirectXTest\\source\\Library\\Content\\Arial_14_Regular.spritefont");
		std::cout << "Sprite font created." << std::endl;

		Game::Initialize();
	}

	void RenderingGame::Shutdown()
	{
		DeleteObject(mFpsComponent);
		DeleteObject(mKeyboard);
		DeleteObject(mMouse);
		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);

		ReleaseObject(mDirectInput);

		Game::Shutdown();
	}

	void RenderingGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		mSpriteBatch->Begin();

		std::wostringstream mouseLabel;
		mouseLabel << L"Mouse Position: " << mMouse->X() << ", " << mMouse->Y() << " Mouse Wheel: " << mMouse->Wheel();
		mSpriteFont->DrawString(mSpriteBatch, mouseLabel.str().c_str(), mMouseTextPosition);

		mSpriteBatch->End();

		HRESULT hr = mSwapChain->Present(0, 0);

		if (FAILED(hr))
		{
			throw GameException("IDXGISwapChain::Present() failed.", hr);
		}
	}
}