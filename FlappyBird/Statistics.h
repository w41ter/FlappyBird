#pragma once

#include "UIBase.h"
#include "BaseStateMachine.h"
#include "Graphics.h"

namespace games
{

    class Statistics : public BaseStateMachine
    {
        Statistics();
        virtual ~Statistics();
    public:

        static Statistics &instance();

        void LastScore(int score);
        void TrainLastScore(int score);

        virtual void Run();
        virtual void OnRender();
        virtual void OnRenderUI();
        virtual void OnMouseEvent(MouseEvent event, int x, int y);
        virtual void OnKeywordEvent(KeywordEvent event, unsigned int state);
    private:

        void UpdateGround(float ElapsedTime);

    private:
        UIState state;

        int groundHeight;
        float groundOffset;

        std::wstring imgBackground;
        std::wstring imgGround;
        std::wstring configFileName;

        FontHandle msgFont;
        FontHandle tipsFont;

        int max_;
        int last_;
        int trainMax_;
        int trainLast_;
    };

}