#pragma once

#include "UIBase.h"
#include "BaseStateMachine.h"
#include "Graphics.h"

namespace games
{

    class UIPanelState : public BaseStateMachine
    {
        UIPanelState();
        virtual ~UIPanelState();
    public:

        static UIPanelState &instance();

        virtual void Run();
        virtual void OnRender();
        virtual void OnRenderUI();
        virtual void OnMouseEvent(MouseEvent event, int x, int y);
        virtual void OnKeywordEvent(KeywordEvent event, unsigned int state);

    private:
        void UpdateGround(float ElapsedTime);
        void BeginGames();
        void TrainComputer();
        void Statistics();

    private:
        UIState state;

        int groundHeight;
        float groundOffset;

        std::wstring imgBackground;
        std::wstring imgGround;

        FontHandle selectButtonFont;
        FontHandle tipsFont;
    };
}
