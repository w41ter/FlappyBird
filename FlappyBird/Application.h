#ifndef _RPG_FRAMEWORK_APPLICATION_H_
#define _RPG_FRAMEWORK_APPLICATION_H_

namespace games
{
	using std::wstring;
	using std::vector;

	enum class MouseEvent
	{
		LEFT_DOWN,
		LEFT_UP,
		LEFT_DBCLICK,	// Ë«»÷
		MIDDLE_DOWN,
		MIDDLE_UP,
		MIDDLE_DBCLICK,	// Ë«»÷
		RIGHT_DOWN,
		RIGHT_UP,
		RIGHT_DBCLICK,	// Ë«»÷
		MOVE,
	};

	enum class KeywordEvent
	{
		KEYDOWN,
		KEYRUP,
	};

    struct APPConfig {
        const wchar_t *title_;
        int height_;
        int width_;
        int frame_;
    };

	class Application
	{
		using MouseCallback = std::function<void(MouseEvent, int, int)>;
		using KeywordCallback = std::function<void(KeywordEvent, unsigned int)>;
        using MessageRun = std::function<void(void)>;

	public:

        HRESULT initialize();
        void setConfig(APPConfig &config);
        void exit();
        HWND getInstance() { return hwnd_; }

		// Process and dispatch messages
		void runMessageLoop(MessageRun running);

        // Event rigister.
        void mouseEventRegister(MouseCallback callback);
        void keywordEventRegister(KeywordCallback callback);
        void releaseEventRegister();

        static Application &Instance();

	private:

        Application();
        ~Application();

		HRESULT CreateApp();

		// Draw content.
		HRESULT OnRender();

		// Mouse and keyword event.
		void OnMouseEvent(MouseEvent event, int x, int y);
		void OnKeywordEvent(KeywordEvent event, unsigned int state);

		// Resize the render target.
		void OnResize(UINT width_, UINT height_);
		//void OnTimer(UINT id);

		// The windows procedure.
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HWND hwnd_;

        APPConfig appConfig_;

		// event callback list.
		vector<MouseCallback> mouseEventCallbackList_;
		vector<KeywordCallback> keywordEventCallbackList_;
	};
}


#endif	// _RPG_FRAMEWORK_APPLICATION_H_