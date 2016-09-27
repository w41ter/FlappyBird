#include "main.h"
#include "Statistics.h"

#include "Ulit.h"
#include "constant.h"
#include "Graphics.h"
#include "Application.h"
#include "StateMachine.h"
#include <sstream>
#include <fstream>

namespace games
{

    Statistics::Statistics()
    {
        msgFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 18);
        tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 14);
        Graphics::Instance().FontCenter(tipsFont);

        std::wstring &path = GetAppPath();

        // 初始化素材部分
        imgBackground = path + L"Resource\\background.png";
        imgGround = path + L"Resource\\Ground.png";

        groundHeight = MS_GROUNDHEIGHT;
        groundOffset = 0.f;

        configFileName = path + L"Resource\\score.txt";
        std::wfstream file(configFileName, std::ios::in);

        if (file)
        {
            file >> max_ >> last_ >> trainLast_ >> trainMax_;
        }
        else
        {
            max_ = 0;
            last_ = 0;
            trainLast_ = 0;
            trainMax_ = 0;
        }
        ClearState(state);
    }


    Statistics::~Statistics()
    {
        std::wfstream file(configFileName, std::ios::out);

        if (file)
        {
            file << max_ << " " << last_ << " " << trainLast_ << " " << trainMax_;
        }

        Graphics::Instance().DestroyFontObject(msgFont);
        Graphics::Instance().DestroyFontObject(tipsFont);
    }

    Statistics & Statistics::instance()
    {
        static Statistics stat;
        return stat;
    }

    void Statistics::LastScore(int score)
    {
        if (max_ < score)
            max_ = score;
        last_ = score;
    }

    void Statistics::TrainLastScore(int score)
    {
        if (trainMax_ < score)
            trainMax_ = score;
        trainLast_ = score;
    }

    void Statistics::Run()
    {
        static DWORD last = GetTickCount();
        DWORD now = GetTickCount();
        float use = (now - last) / 1000.f;
        last = now;
        UpdateGround(use);
    }

    void Statistics::OnRender()
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

    void Statistics::OnRenderUI()
    {
        int width = 300, height = 200;
        float left = (GAMES_SIZE_W - width) >> 1;
        float top = (GAMES_SIZE_H - height - 100) >> 1;
        auto rect = D2D1::RoundedRect(
            D2D1::RectF(left, top, left + width, top + height),
            3.f, 3.f
            );
        Graphics::Instance().FillRoundedRectangle(rect, 0xf0f000, 0.5);

        std::wstringstream stream;
        stream << L"最高分: " << max_ << std::endl;
        stream << L"上次得分: " << last_ << std::endl;
        stream << L"电脑最高分: " << trainMax_ << std::endl;
        stream << L"电脑上次得分: " << trainLast_ << std::endl;
        std::wstring msg = stream.str();
        Graphics::Instance().DrawText(
            msg.c_str(),
            msg.length(),
            msgFont,
            D2D1::RectF(left + 20, top + 20, left + width - 40, top + height - 70),
            0);
        msg = L"按空格继续";
        Graphics::Instance().DrawText(
            msg.c_str(),
            msg.length(),
            tipsFont,
            D2D1::RectF(left + 20, top + height - 70, left + width - 40, top + height - 20),
            0);
    }

    void Statistics::OnMouseEvent(MouseEvent event, int x, int y)
    {
    }

    void Statistics::OnKeywordEvent(KeywordEvent event, unsigned int state)
    {
        if (event == KeywordEvent::KEYDOWN && state == VK_SPACE)
        {
            StateMachine::Instance().NextState(GameStatus::UI_PANEL_STATUS);
        }
    }

    void Statistics::UpdateGround(float ElapsedTime)
    {
        // 移动地板
        groundOffset = (float)fmod(groundOffset + ElapsedTime * MS_STAGEMOVESPEED, MS_GROUNDWIDTH);
    }
}