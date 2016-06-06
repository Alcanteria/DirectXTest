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
#include "..\Library\MatrixHelper.h"

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = ColorHelper::CornflowerBlue;

	RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
		: Game(instance, windowClass, windowTitle, showCommand), mFpsComponent(nullptr), mDirectInput(nullptr), mKeyboard(nullptr), mMouse(nullptr), mRenderStateHelper(nullptr), mGrid(nullptr), mSpotLightDemo(nullptr),
		mRenderTarget(nullptr), mFullScreenQuad(nullptr), mColorFilterEffect(nullptr), mColorFilterMaterial(nullptr), mSkyBox(nullptr), mActiveColorFilter(ColorFilterGrayScale), mGenericColorFilter(MatrixHelper::Identity),
		mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 16.0f)
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

		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		mKeyboard = new Keyboard(*this, mDirectInput);
		mComponents.push_back(mKeyboard);
		mServices.AddService(Keyboard::TypeIdClass(), mKeyboard);

		mMouse = new Mouse(*this, mDirectInput);
		mComponents.push_back(mMouse);
		mServices.AddService(Mouse::TypeIdClass(), mMouse);

		mSpriteBatch = new SpriteBatch(this->Direct3DDeviceContext());
		mSpriteFont = new SpriteFont(this->Direct3DDevice(), L"..\\source\\Library\\Content\\Arial_14_Regular.spritefont");

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
		//mFullScreenQuad->SetActiveTechnique("grayscale_filter", "p0");
		//mFullScreenQuad->SetActiveTechnique("inverse_filter", "p0");
		mFullScreenQuad->SetActiveTechnique(ColorFilterTechniqueNames[mActiveColorFilter], "p0");
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

		DeleteObject(mSpriteBatch)
		DeleteObject(mSpriteFont)
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

		if (mKeyboard->WasKeyPressedThisFrame(DIK_RETURN))
		{
			mActiveColorFilter = ColorFilter(mActiveColorFilter + 1);
			if (mActiveColorFilter >= ColorFilterEnd)
			{
				mActiveColorFilter = (ColorFilter)(0);
			}

			mFullScreenQuad->SetActiveTechnique(ColorFilterTechniqueNames[mActiveColorFilter], "p0");
		}

		UpdateGenericColorFilter(gameTime);

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

		mRenderStateHelper->SaveAll();

		mSpriteBatch->Begin();

		std::wostringstream helpLabel;
		helpLabel << L"Ambient Intensity (+PgUp/-PgDn): " << mSpotLightDemo->GetAmbientColor().a << "\n";
		helpLabel << L"Point Light Intensity (+Home/-End): " << mSpotLightDemo->GetSpotLightColor().a << "\n";
		helpLabel << L"Specular Power (+Insert/-Delete): " << mSpotLightDemo->GetSpecularColor().a << "\n";
		helpLabel << L"Move Point Light (8/2, 4/6, 3/9)\n";
		//helpLabel << std::setprecision(2) << L"Active Filter (Enter): " << ColorFilterDisplayNames[mActiveColorFilter].c_str();
		helpLabel << L"Active Filter (Enter): " << ColorFilterDisplayNames[mActiveColorFilter].c_str();
		if (mActiveColorFilter == ColorFilterGeneric)
		{
			helpLabel << L"\nBrightness (+Comma/-Period): " << mGenericColorFilter._11 << "\n";
		}

		mSpriteFont->DrawString(mSpriteBatch, helpLabel.str().c_str(), mTextPosition);

		mSpriteBatch->End();

		mRenderStateHelper->RestoreAll();

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
		XMMATRIX colorFilter = XMLoadFloat4x4(&mGenericColorFilter);

		mColorFilterMaterial->ColorTexture() << mRenderTarget->OutputTexture();
		mColorFilterMaterial->ColorFilter() << colorFilter;
	}

	void RenderingGame::UpdateGenericColorFilter(const GameTime& gameTime)
	{
		static float brightness = 1.0f;
		static float BrightnessModulationRate = 0.5f;

		if (mKeyboard != nullptr)
		{
			if (mKeyboard->IsKeyDown(DIK_COMMA) && brightness < 1.0f)
			{
				brightness += BrightnessModulationRate * (float)gameTime.ElapsedGameTime();
				brightness = XMMin<float>(brightness, 1.0f);
				XMStoreFloat4x4(&mGenericColorFilter, XMMatrixScaling(brightness, brightness, brightness));
			}

			if (mKeyboard->IsKeyDown(DIK_PERIOD) && brightness > 0.0f)
			{
				brightness -= BrightnessModulationRate * (float)gameTime.ElapsedGameTime();
				brightness = XMMax<float>(brightness, 0.0f);
				XMStoreFloat4x4(&mGenericColorFilter, XMMatrixScaling(brightness, brightness, brightness));
			}
		}
	}
}