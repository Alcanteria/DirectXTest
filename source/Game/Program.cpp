#include "stdafx.h"
#include <memory>
#include "..\Library\GameException.h"
#include "RenderingGame.h"
#include "GaussianBlurGame.h"
#include "BloomGame.h"
#include "DistortionMappingGame.h"
#include "DistortionMappingPostGame.h"


#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

using namespace Library;
using namespace Rendering;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	std::unique_ptr<RenderingGame>game(new RenderingGame(instance, L"RenderingClass", L"Real-Time 3D Rendering", showCommand));
	//std::unique_ptr<GaussianBlurGame>game(new GaussianBlurGame(instance, L"RenderingClass", L"Real-Time 3D Rendering", showCommand));
	//std::unique_ptr<DistortionMappingGame>game(new DistortionMappingGame(instance, L"RenderingClass", L"Real-Time 3D Rendering", showCommand));
	//std::unique_ptr<DistortionMappingPostGame>game(new DistortionMappingPostGame(instance, L"RenderingClass", L"Real-Time 3D Rendering", showCommand));

	try
	{
		game->Run();
	}
	catch (GameException ex)
	{
		MessageBox(game->WindowHandle(), ex.whatw().c_str(), game->WindowTitle().c_str(), MB_ABORTRETRYIGNORE);
	}

	return 0;
}