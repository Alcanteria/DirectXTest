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
#include "..\Library\ColorHelper.h"
#include "..\Library\RenderStateHelper.h"
#include "..\Library\RasterizerStates.h"
#include "..\Library\Grid.h"
#include "..\Library\SamplerStates.h"
#include "MaterialDemo.h"
#include "..\Library\FirstPersonCamera.h"
#include <iostream>
#include "..\Library\Skybox.h"
#include "SpotLightDemo.h"

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = ColorHelper::CornflowerBlue;

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
		: Game(instance, windowClass, windowTitle, showCommand), mFpsComponent(nullptr), mDirectInput(nullptr), mKeyboard(nullptr), mMouse(nullptr), mRenderStateHelper(nullptr), mGrid(nullptr), mSpotLightDemo(nullptr),
		mRenderTarget(nullptr), mFullScreenQuad(nullptr), mColorFilterEffect(nullptr), mColorFilterMaterial(nullptr), mSkyBox(nullptr)
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

		mCamera = new FirstPersonCamera(*this);
		mComponents.push_back(mCamera);
		mServices.AddService(Camera::TypeIdClass(), mCamera);

		mFpsComponent = new FpsComponent(*this);
		mFpsComponent->Initialize();
		//mComponents.push_back(mFpsComponent);

		mGrid = new Grid(*this, *mCamera);
		mComponents.push_back(mGrid);

		mSkyBox = new Skybox(*this, *mCamera, L"..\\source\\Library\\Content\\Textures\\BaconTextureCube.dds", 100.0f);
		mComponents.push_back(mSkyBox);

		RasterizerStates::Initialize(mDirect3DDevice);
		SamplerStates::BorderColor = ColorHelper::Black;
		SamplerStates::Initialize(mDirect3DDevice);

		mSpotLightDemo = new SpotLightDemo(*this, *mCamera);
		mComponents.push_back(mSpotLightDemo);

		mRenderStateHelper = new RenderStateHelper(*this);

		/* POST PROCESSING STUFF */
		mRenderTarget = new FullScreenRenderTarget(*this);
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
		mColorFilterEffect = new Effect(*this);
		mColorFilterEffect->LoadCompiledEffect(L"Content\\Effects\\ColorFilter.cso");

		mColorFilterMaterial = new ColorFilterMaterial();
		mColorFilterMaterial->Initialize(*mColorFilterEffect);

		mFullScreenQuad = new FullScreenQuad(*this, *mColorFilterMaterial);
		mFullScreenQuad->Initialize();
		mFullScreenQuad->SetActiveTechnique("grayscale_filter", "p0");
		mFullScreenQuad->SetCustomUpdateMaterial(std::bind(&RenderingGame::UpdateColorFilterMaterial, this));
		/* POST PROCESSING STUFF */

		Game::Initialize();

		mCamera->SetPosition(0.0f, 0.0f, 25.0f);
	}

	void RenderingGame::Shutdown()
	{
		DeleteObject(mSpotLightDemo);

		/* POST PROCESSING STUFF */
		DeleteObject(mRenderTarget);
		DeleteObject(mFullScreenQuad);
		DeleteObject(mColorFilterEffect);
		DeleteObject(mColorFilterMaterial);
		/* POST PROCESSING STUFF */

		DeleteObject(mGrid)
		DeleteObject(mSkyBox)
		DeleteObject(mRenderStateHelper);
		DeleteObject(mKeyboard);
		DeleteObject(mMouse);
		DeleteObject(mFpsComponent);
		DeleteObject(mCamera);

		ReleaseObject(mDirectInput);
		RasterizerStates::Release();
		SamplerStates::Release();

		Game::Shutdown();
	}

	void RenderingGame::Update(const GameTime &gameTime)
	{
		mFpsComponent->Update(gameTime);

		if (mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const GameTime &gameTime)
	{

		/*POST PROCESSING VERSION*/

		mRenderTarget->Begin();

		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTarget->RenderTargetView(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mRenderTarget->DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		mRenderTarget->End();

		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		mFullScreenQuad->Draw(gameTime);

		HRESULT hr = mSwapChain->Present(0, 0);

		if (FAILED(hr))
		{
			throw GameException("IDXGISwapChain::Present() failed.", hr);
		}

		/*POST PROCESSING VERSION*/


		/*mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		mRenderStateHelper->SaveAll();
		mFpsComponent->Draw(gameTime);
		mRenderStateHelper->RestoreAll();

		HRESULT hr = mSwapChain->Present(0, 0);

		if (FAILED(hr))
		{
			throw GameException("IDXGISwapChain::Present() failed.", hr);
		}*/
	}

	void RenderingGame::UpdateColorFilterMaterial()
	{
		mColorFilterMaterial->ColorTexture() << mRenderTarget->OutputTexture();
	}
}