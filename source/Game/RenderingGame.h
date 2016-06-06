#pragma once

#include "..\Library\Common.h"
#include "..\Library\Game.h"
#include "..\Library\FullScreenRenderTarget.h"
#include "..\Library\FullScreenQuad.h"
#include "..\Library\ColorFilterMaterial.h"

using namespace Library;

namespace Library
{
	class Keyboard;
	class Mouse;
	class FirstPersonCamera;
	class FpsComponent;
	class RenderStateHelper;
	class Grid;
	class Skybox;
	class FullScreenRenderTarget;
	class FullScreenQuad;
	class Effect;
}

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

namespace Rendering
{
	class SpotLightDemo;
	class ColorFilterMaterial;

	enum ColorFilter
	{
		ColorFilterGrayScale = 0,
		ColorFilterInverse,
		ColorFilterSepia,
		ColorFilterGeneric,
		ColorFilterEnd
	};

	const std::string ColorFilterTechniqueNames[] = { "grayscale_filter", "inverse_filter", "sepia_filter", "generic_filter" };
	const std::string ColorFilterDisplayNames[] = { "Grayscale", "Inverse", "Sepia", "Generic" };

	class RenderingGame : public Game
	{
	public:
		RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring&  windowTitle, int showCommand);
		~RenderingGame();

		virtual void Initialize() override;
		virtual void Update(const GameTime& gameTime) override;
		virtual void Draw(const GameTime& gameTime) override;

		/* POST PROCESSING STUFF. */
		void UpdateColorFilterMaterial();
		void UpdateGenericColorFilter(const GameTime& gameTime);
		/* POST PROCESSING STUFF. */

	protected:
		virtual void Shutdown() override;

	private:
		static const XMVECTORF32 BackgroundColor;

		LPDIRECTINPUT8 mDirectInput;
		Keyboard* mKeyboard;
		Mouse* mMouse;
		FirstPersonCamera* mCamera;
		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;
		Grid* mGrid;

		SpotLightDemo* mSpotLightDemo;
		Skybox* mSkyBox;

		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;
		XMFLOAT2 mTextPosition;

		/* POST PROCESSING STUFF. */
		FullScreenRenderTarget* mRenderTarget;
		FullScreenQuad* mFullScreenQuad;
		Effect* mColorFilterEffect;
		ColorFilterMaterial* mColorFilterMaterial;

		ColorFilter mActiveColorFilter;
		XMFLOAT4X4 mGenericColorFilter;
		/* POST PROCESSING STUFF. */
	};
}