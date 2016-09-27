#include "main.h"
#include "Application.h"
#include "StateMachine.h"
#include "Graphics.h"
#include "TimeCounter.h"
#include <time.h>

using games::Application;
using games::StateMachine;
using games::Graphics;
using games::TimeCounter;

int g_FPS = 0;
int g_FPSCount = 0;
games::FontHandle g_FPSFont;
D2D1_RECT_F g_FPSRect{ 0, 0, 100, 30 };

void init();
void MessageLoop();
int getFPS();

int WINAPI wWinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPWSTR /* lpCmdLine */,
	int /* nCmdShow */
	)
{
	// Use HeapSetInformation to specify that the process should
	// terminate if the heap manager detects an error in any heap used
	// by the process.
	// The return value is ignored, because we want to continue running in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    srand(time(NULL));

	if (SUCCEEDED(CoInitialize(NULL)))
	{
        games::APPConfig config;
        config.title_ = TEXT("Flappy Bird");
        config.height_ = 600;
        config.width_ = 800;
        config.frame_ = 65;

		Application &app = Application::Instance();
        app.setConfig(config);

        if (SUCCEEDED(app.initialize()))
        {
            init();
            app.runMessageLoop([] { MessageLoop(); });
        }

		CoUninitialize();
	}

	return 0;
}

int getFPS()
{
    return g_FPS;
}

void init()
{
    int id = TimeCounter::instance().Schedule(0, 1000.f, [] { g_FPS = g_FPSCount; g_FPSCount = 0; });
    g_FPSFont = Graphics::Instance().CreateFontObject(TEXT("Î¢ÈíÑÅºÚ"), 12.f);
    Graphics::Instance().FontCenter(g_FPSFont);

    Graphics::Instance().RegisterRenderCallback(games::RenderLevel::THIRED, [] {
        // show fps in left cornor.
        TCHAR temp[32] = { 0 };
        wsprintf(temp, TEXT("FPS: %d"), getFPS());
        Graphics::Instance().DrawText(temp, lstrlenW(temp), g_FPSFont, g_FPSRect, RGB(256, 256, 256));
    });
}

void MessageLoop()
{
    ++g_FPSCount;

    StateMachine::Instance().Run();

}