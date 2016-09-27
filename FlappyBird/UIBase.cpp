#include "main.h"

#include "UIBase.h"

#include "Application.h"

namespace games
{
	bool RegionHit(UIState &state, D2D1_RECT_F rc)
	{
		if (state.mousex < rc.left ||
			state.mousey < rc.top  ||
			state.mousex > rc.right ||
			state.mousey > rc.bottom)
			return false;
		return true;
	}

	bool RegionHit(UIState & state, float left, float top, float right, float bottom)
	{
		if (state.mousex < left ||
			state.mousey < top ||
			state.mousex > right ||
			state.mousey > bottom)
			return false;
		return true;
	}

	void UIPrepare(UIState & state)
	{
		state.hotitem = 0;
	}

	void UIFinish(UIState & state)
	{
		if (state.mousedown == 0)
		{
			state.activeitem = 0;
		}
		else
		{
			if (state.activeitem == 0)
				state.activeitem = -1;
		}
	}

	void ClearState(UIState & state)
	{
		state.activeitem = -1;
		state.hotitem = 0;
		state.mousedown = 0;
	}

    void InitUIState(UIState & state, MouseEvent event, int x, int y)
    {
        switch (event)
        {
        case MouseEvent::LEFT_DOWN:
            state.mousedown = 1;
            break;
        case MouseEvent::LEFT_UP:
            state.mousedown = 0;
            break;
        case MouseEvent::MOVE:
            state.mousex = x;
            state.mousey = y;
            break;
        }
    }
}