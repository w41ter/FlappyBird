#pragma once

namespace games
{
	typedef int UIID;

    enum class MouseEvent;

	struct UIState
	{
		int mousex;
		int mousey;
		int mousedown;

		int hotitem;
		int activeitem;
	};

	bool RegionHit(UIState &state, D2D1_RECT_F rc);
	bool RegionHit(UIState &state, float left, float top, float right, float bottom);

	void UIPrepare(UIState &state);
	void UIFinish(UIState &state);

	void ClearState(UIState &state);

    void InitUIState(UIState &state, MouseEvent event, int x, int y);
}
