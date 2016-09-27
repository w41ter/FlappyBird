#include "main.h"

#include "Application.h"

#include <thread>

#include "Graphics.h"
#include "TimeCounter.h"
#include "MessageQueue.h"

#include "resource.h"

namespace games
{
 
	Application::Application() :
		hwnd_(NULL)
	{
	}

	Application::~Application()
	{
	}

	Application & Application::Instance()
	{
		static Application app;
		return app;
	}

	void Application::runMessageLoop(MessageRun running)
	{
        assert(running);

        TimeCounter::instance().Schedule(1, 1000.f / appConfig_.frame_,
            [this, running] {
            running();
            OnRender(); 
        });

        MSG msg;
		// Main message loop:
		while (TRUE)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
                TimeCounter::instance().Update();
                if (MessageQueue::instance().HasMessage())
                    MessageQueue::instance().GetMessage()();

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	void Application::mouseEventRegister(MouseCallback callback)
	{
		mouseEventCallbackList_.push_back(callback);
	}

	void Application::keywordEventRegister(KeywordCallback callback)
	{
		keywordEventCallbackList_.push_back(callback);
	}

	void Application::releaseEventRegister()
	{
		mouseEventCallbackList_.clear();
		keywordEventCallbackList_.clear();
	}

    void Application::exit()
    {
        PostQuitMessage(0); 
    }

	HRESULT Application::CreateApp()
	{
		HRESULT hr;

		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Application::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.hIcon = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_ICON));
        wcex.hIconSm = LoadIconW(wcex.hInstance, MAKEINTRESOURCEW(IDI_ICON));
		wcex.lpszClassName = TEXT("Mario");

		RegisterClassEx(&wcex);

		DWORD style = WS_OVERLAPPEDWINDOW;
		style &= ~WS_MAXIMIZEBOX; //禁止窗口最大化
		//style &= ~WS_MINIMIZEBOX; //禁止窗口最小化
		//style &= ~WS_SYSMENU; //取消Title上的按钮
		style &= ~WS_THICKFRAME;//使窗口不能用鼠标改变大小

		// Create the window.
		hwnd_ = CreateWindow(
            TEXT("Mario"),
			appConfig_.title_,
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
            appConfig_.width_,
            appConfig_.height_,
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
			);
		hr = hwnd_ ? S_OK : E_FAIL;
		return hr;
	}

    void Application::setConfig(APPConfig &config)
    {
        memcpy(&appConfig_, &config, sizeof(APPConfig));
    }

    HRESULT Application::initialize()
	{
		HRESULT hr;

		hr = CreateApp();
		if (SUCCEEDED(hr))
			// initialize Graphics resources.
			hr = Graphics::Instance().initialize(hwnd_);

		if (SUCCEEDED(hr))
		{
			ShowWindow(hwnd_, SW_SHOWNORMAL);
			UpdateWindow(hwnd_);
		}

		return hr;
	}

	HRESULT Application::OnRender()
	{
		HRESULT hr = S_OK;

		hr = Graphics::Instance().OnRender();

		return hr;
	}

	void Application::OnMouseEvent(MouseEvent event, int x, int y)
	{
		for (auto &i : mouseEventCallbackList_)
			i(event, x, y);
	}

	void Application::OnKeywordEvent(KeywordEvent event, unsigned int state)
	{
		for (auto &i : keywordEventCallbackList_)
			i(event, state);
	}


	LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		if (message == WM_CREATE)
		{
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			Application *pApp = (Application *)pcs->lpCreateParams;

			::SetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA,
				PtrToUlong(pApp)
				);

			result = 1;
		}
		else
		{
			Application *pApp = reinterpret_cast<Application *>(static_cast<LONG_PTR>(
				::GetWindowLongPtrW(
					hwnd,
					GWLP_USERDATA
					)));

			bool wasHandled = false;

			if (pApp)
			{
				switch (message)
				{
				case WM_SIZE:
				{
					UINT width_ = LOWORD(lParam);
					UINT height_ = HIWORD(lParam);
					pApp->OnResize(width_, height_);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_DISPLAYCHANGE:
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_PAINT:
				{
					pApp->OnRender();
					ValidateRect(hwnd, NULL);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				result = 1;
				wasHandled = true;
				break;
				
				case WM_LBUTTONDOWN:
				{
					pApp->OnMouseEvent(MouseEvent::LEFT_DOWN, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_LBUTTONUP:
				{
					pApp->OnMouseEvent(MouseEvent::LEFT_UP, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_LBUTTONDBLCLK:
				{
					pApp->OnMouseEvent(MouseEvent::LEFT_DBCLICK, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_RBUTTONDOWN:
				{
					pApp->OnMouseEvent(MouseEvent::RIGHT_DOWN, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_RBUTTONUP:
				{
					pApp->OnMouseEvent(MouseEvent::RIGHT_UP, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_RBUTTONDBLCLK:
				{
					pApp->OnMouseEvent(MouseEvent::RIGHT_DBCLICK, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_MBUTTONDOWN:
				{
					pApp->OnMouseEvent(MouseEvent::MIDDLE_DOWN, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_MBUTTONUP:
				{
					pApp->OnMouseEvent(MouseEvent::MIDDLE_UP, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_MBUTTONDBLCLK:
				{
					pApp->OnMouseEvent(MouseEvent::MIDDLE_DBCLICK, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_MOUSEMOVE:
				{
					pApp->OnMouseEvent(MouseEvent::MOVE, LOWORD(lParam), HIWORD(lParam));
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_KEYDOWN:
				{
					pApp->OnKeywordEvent(KeywordEvent::KEYDOWN, wParam);
				}
				result = 0;
				wasHandled = true;
				break;

				case WM_KEYUP:
				{
					pApp->OnKeywordEvent(KeywordEvent::KEYRUP, wParam);
				}
				result = 0;
				wasHandled = true;
				break;
				
				result = 0;
				wasHandled = true;
				break;
				}
			}

			if (!wasHandled)
			{
				result = DefWindowProc(hwnd, message, wParam, lParam);
			}
		}

		return result;
	}

	void Application::OnResize(UINT width_, UINT height_)
	{
		Graphics::Instance().OnResize(width_, height_);
	}


}
