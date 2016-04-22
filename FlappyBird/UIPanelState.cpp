#include "main.h"
#include "UIPanelState.h"

#include "UIControl.h"
#include "constant.h"
#include "Ulit.h"
#include "StateMachine.h"
#include "GameState.h"

namespace games
{

    UIPanelState::UIPanelState()
    {
        selectButtonFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 20);
        tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 14);
        Graphics::Instance().FontCenter(selectButtonFont);
        Graphics::Instance().FontCenter(tipsFont);

        std::wstring &path = GetAppPath();

        // 初始化素材部分
        imgBackground = path + L"Resource\\background.png";
        imgGround = path + L"Resource\\Ground.png";

        groundHeight = MS_GROUNDHEIGHT;
        groundOffset = 0.f;

        ClearState(state);
    }

    UIPanelState::~UIPanelState()
    {
        Graphics::Instance().DestroyFontObject(selectButtonFont);
        Graphics::Instance().DestroyFontObject(tipsFont);
    }

    UIPanelState & UIPanelState::instance()
    {
        static UIPanelState state;
        return state;
    }

    void UIPanelState::UpdateGround(float ElapsedTime)
    {
        // 移动地板
        groundOffset = (float)fmod(groundOffset + ElapsedTime * MS_STAGEMOVESPEED, MS_GROUNDWIDTH);
    }

    void UIPanelState::BeginGames()
    {
        GameState::instance().setAIEnable(false);
        StateMachine::Instance().NextState(GameStatus::GAME_STATUS);
    }

    void UIPanelState::TrainComputer()
    {
        GameState::instance().setAIEnable();
        StateMachine::Instance().NextState(GameStatus::GAME_STATUS);
    }

    void UIPanelState::Statistics()
    {
        StateMachine::Instance().NextState(GameStatus::UI_STATISTICS);
    }

    void UIPanelState::Run()
    {
        static DWORD last = GetTickCount();
        DWORD now = GetTickCount();
        float use = (now - last) / 1000.f;
        last = now;
        UpdateGround(use);
    }

    void UIPanelState::OnRender()
    {
        D2D1_RECT_F rect{ 0, 0, 800, 600 };
        Graphics::Instance().DrawBitmap(imgBackground, rect);

        // 绘制地板
        for (int i = 0; i <= MS_GROUNDDRAWCOUNT; ++i)
        {
            D2D1_RECT_F ground{
                MS_GROUNDWIDTH * i,
                GAMES_SIZE_H - groundHeight,
                MS_GROUNDWIDTH * i + MS_GROUNDWIDTH,
                GAMES_SIZE_H
            };
            Graphics::Instance().DrawBitmap(imgGround, ground);
        }
    }

    void UIPanelState::OnRenderUI()
    {
        UIPrepare(state);

        float left = (GAMES_SIZE_W - 100) >> 1;
        float top = (GAMES_SIZE_H - 150 - 100) >> 1;
        float right = left + 100;
        float bottom = top + 40;

        if (UIControl::TextButton(state, 1, selectButtonFont, L"开始游戏", left, top, right, bottom))
        {
            BeginGames();
        }

        top += 50;
        bottom = top + 40;
        if (UIControl::TextButton(state, 2, selectButtonFont, L"电脑训练", left, top, right, bottom))
        {
            TrainComputer();
        }

        top += 50;
        bottom = top + 40;
        if (UIControl::TextButton(state, 3, selectButtonFont, L"统计数据", left, top, right, bottom))
        {
            Statistics();
        }

        UIFinish(state);
    }

    void UIPanelState::OnMouseEvent(MouseEvent event, int x, int y)
    {
        InitUIState(state, event, x, y);
    }

    void UIPanelState::OnKeywordEvent(KeywordEvent event, unsigned int state)
    {
    }
}