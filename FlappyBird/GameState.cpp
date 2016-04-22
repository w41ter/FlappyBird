#include "main.h"
#include "GameState.h"

#include "Graphics.h"

#include "Animation.h"
#include "Sprite.h"
#include "Ulit.h"
#include "Application.h"
#include "TimeCounter.h"
#include "Collision.h"
#include "Statistics.h"
#include "StateMachine.h"
#include "QLearning.h"
#include "constant.h"

using games::KeywordEvent;

namespace games
{
    double TimeTick() 
    {
        static double last = TimeCounter::instance().getTick();
        double now = TimeCounter::instance().getTick();
        double use = (now - last) / 1000;
        last = now;
        return use;
    }

    GameState::GameState()
    {
        tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 14);
        Graphics::Instance().FontCenter(tipsFont);
        scoreFont = Graphics::Instance().CreateFontObject(TEXT("微软雅黑"), 26.f);
        Graphics::Instance().FontCenter(scoreFont);

        initialize();

        ai = std::make_shared<QLearning>();
    }

    GameState::~GameState()
    {
        Graphics::Instance().DestroyFontObject(tipsFont);
        Graphics::Instance().DestroyFontObject(scoreFont);
    }

    GameState & GameState::instance()
    {
        static GameState game;
        return game;
    }

    void GameState::initialize()
    {
        std::wstring &path = GetAppPath();

        // 初始化素材部分
        imgBackground = path + L"Resource\\background.png";
        imgGround = path + L"Resource\\Ground.png";
        imgPipeTop = path + L"Resource\\pipe_top.png";
        imgPipeBottom = path + L"Resource\\pipe_bottom.png";
        imgGetReady = path + L"Resource\\ready.png";
        imgGamesOver = path + L"Resource\\gamesover.png";

        D2D1_SIZE_F size;
        Graphics::Instance().GetBitmapSize(imgGetReady, size);

        getReadImageSize = D2D1::RectF(
            (GAMES_SIZE_W - size.width) / 2,
            (GAMES_SIZE_H - size.height) / 2,
            (GAMES_SIZE_W + size.width) / 2,
            (GAMES_SIZE_H + size.height) / 2
        );

        Graphics::Instance().GetBitmapSize(imgGamesOver, size);

        gameOverImageSize = {
            (GAMES_SIZE_W - size.width) / 2,
            (GAMES_SIZE_H - size.height - 100) / 2,
            (GAMES_SIZE_W + size.width) / 2,
            (GAMES_SIZE_H + size.height - 100) / 2,
        };

        // 初始化小鸟动画
        fly = std::make_shared<Animation>();
        for (int i = 0; i < 3; ++i)
        {
            TCHAR filename[MAX_PATH];
            wsprintf(filename, L"%sResource\\birds\\%d.png", path.c_str(), i);
            fly->addSpriteFrameWithFileName(filename);
        }
        fly->setRestoreOriginalFrame(true);
        fly->setDelayPerUnit(90);

        // 初始化小鸟 sprite
        std::wstring birdImage = path + L"Resource\\birds\\0.png";
        Graphics::Instance().GetBitmapSize(birdImage, birdSize);
        birds = std::make_shared<Sprite>();
        birds->runAction(fly.get());

        Restart();
    }

    void GameState::initBirdPosition()
    {
        birdHeight = birdSize.height;
        birdWidth = birdSize.width;
        birdPos.x = MS_BIRDCENTERX;
        birdPos.y = (GAMES_SIZE_H - MS_GROUNDHEIGHT) / 2.f;
        int left = birdSize.width / 2, top = birdSize.height / 2;
        birds->setPosition(
            birdPos.x - left,
            birdPos.y - top,
            birdPos.x + left,
            birdPos.y + top);
    }

    void GameState::Begin()
    {
        keyDown = false;

        pipeIndex = 0;
        pipeTimer = TimeCounter::instance().Schedule(
            1, MS_PIPECREATETIME * 850, [this] { CreatePipe(); });

        birdPathLoggerTime = 0.f;

        birdV = 0.f;
        birds->runAction(fly.get());
    }

    void GameState::CreatePipe()
    {
        PipeData tData;

        int y = rand() % (int)(MS_PIPECREATEYMAX - MS_PIPECREATEYMIN) + MS_PIPECREATEYMIN;
        // 计算创建位置(空隙中心)
        ++pipeIndex;
        tData.pos.x = GAMES_SIZE_W + MS_PIPEWIDTH / 2.f;
        tData.pos.y = y;
        tData.Index = pipeIndex;
        tData.Passed = false;

        pipes.push_back(tData);
    }

    void GameState::Restart()
    {
        score = 0;

        birdV = 0.f;
        initBirdPosition();
        birds->setRotation(0.f, D2D1::Point2F(birdPos.x, birdPos.y));

        birdPathLoggerTime = 0.f;
        birdPath.clear();

        pipeIndex = 0;
        pipes.clear();

        gameState = GAMESTATE_READY;
        groundOffset = 0.f;
        TimeCounter::instance().Release(pipeTimer);
    }

    void GameState::Died()
    {
        fly->end();
        if (AI)
        {
            Statistics::instance().TrainLastScore(score);
            ai->die();
        }
        else
        {
            Statistics::instance().LastScore(score);
        }
    }

    void GameState::Run()
    {
        float ElapsedTime = TimeTick();
        switch (gameState)
        {
        case games::GAMESTATE_READY:
            FloatBird(ElapsedTime);
            break;
        case games::GAMESTATE_PLAYING:
        {
            UpdateBird(ElapsedTime);
            UpdatePipes(ElapsedTime);
            UpdateGround(ElapsedTime);
            UpdatePathLogger(ElapsedTime);

            // check
            CheckScore();

            // 碰撞检测
            int tPipeIndex;
            collisionResult = CheckCollision(tPipeIndex);

            if (AI)
            {
                float h_dis = 9999, v_dis = -9999, vg_dis = -9999;
                GetBirdState(h_dis, v_dis, vg_dis);
                if (collisionResult)
                    ai->run(h_dis, v_dis, vg_dis, 0);
                else //if (h_dis < MS_STAGEMOVESPEED * MS_PIPECREATETIME)
                    ai->run(h_dis, v_dis, vg_dis, 1);
            }

            switch (collisionResult)
            {
            case COLLISIONRESULT_TOP_PIPE:
            case COLLISIONRESULT_BOTTOM_PIPE:
                gameState = GAMESTATE_DIYING;
                break;
            case COLLISIONRESULT_GROUND:
                 gameState = GAMESTATE_DIED;
                 Died();
                break;
            }
            break;
        }
        case games::GAMESTATE_DIYING:
        {
            D2D1_ELLIPSE ellipse{
                { birdPos.x, birdPos.y },
                MS_BIRDBOUNDINGCIRCLESIZE,
                MS_BIRDBOUNDINGCIRCLESIZE };

            // 检查与地面的碰撞
            D2D1_POINT_2F p1{ 0, GAMES_SIZE_H - MS_GROUNDHEIGHT },
                p2{ GAMES_SIZE_W, GAMES_SIZE_H - MS_GROUNDHEIGHT };
            if (Collision::CircleLineHitTest(ellipse, p1, p2))
            {
                gameState = GAMESTATE_DIED;
                Died();
            }
            else
            {
                UpdateBird(ElapsedTime);
            }
            break;
        }
        case games::GAMESTATE_DIED:
            break;
        default:
            break;
        }
    }

    void GameState::OnRender()
    {
        DrawMap();
        DrawPipes();
        DrawPath();
        DrawBrid();

#ifdef _DEBUG
        DrawCollisionBox();
#endif
    }

    void GameState::OnRenderUI()
    {
        TCHAR temp[32] = { 0 };
        wsprintf(temp, TEXT("%d"), score);
        Graphics::Instance().DrawText(
            temp, 
            lstrlenW(temp), 
            scoreFont, 
            D2D1::RectF(300, 0, 500, 100), 
            0xffffff);

        if (gameState == GAMESTATE_READY)
        {
            Graphics::Instance().DrawBitmap(imgGetReady, getReadImageSize);
        }
        else if (gameState == GAMESTATE_DIED)
        {
            D2D1_RECT_F rect = gameOverImageSize;
            Graphics::Instance().DrawBitmap(imgGamesOver, rect);

            rect.top = rect.bottom + 10;
            rect.bottom += 40;
            wstring msg = L"按空格继续";
            Graphics::Instance().DrawText(
                msg.c_str(), msg.length(), tipsFont, rect, 0);
        }
    }

    void GameState::OnMouseEvent(MouseEvent event, int x, int y)
    {
    }

    void GameState::OnKeywordEvent(KeywordEvent event, unsigned int state)
    {
        if (gameState == GAMESTATE_PLAYING)
        {
            if (event == KeywordEvent::KEYDOWN && state == VK_SPACE)
            {
                if (!keyDown)
                {
                    birdV = -MS_BIRDJUMPV;
                    keyDown = true;
                }
            }
            else if (event == KeywordEvent::KEYRUP && state == VK_SPACE)
            {
                keyDown = false;
            }
            else if (AI && event == KeywordEvent::KEYDOWN && state == VK_ESCAPE) 
            {
                AI = false;
            }
        }
        else if (gameState == GAMESTATE_READY)
        {
            if (event == KeywordEvent::KEYDOWN && state == VK_SPACE)
            {
                Begin();
                gameState = GAMESTATE_PLAYING;
            }
        }
        else if (gameState == GAMESTATE_DIED)
        {
            if (event == KeywordEvent::KEYDOWN && state == VK_SPACE)
            {
                if (!AI)
                    StateMachine::Instance().NextState(GameStatus::UI_STATISTICS);
                Restart();
            }
        }
    }

    void GameState::setAIEnable(bool state)
    {
        AI = state;
    }

    void GameState::DrawMap()
    {
        D2D1_RECT_F rect{ 0, 0, 800, 600 };
        Graphics::Instance().DrawBitmap(imgBackground, rect);

        // 绘制地板
        for (int i = 0; i <= MS_GROUNDDRAWCOUNT; ++i)
        {
            D2D1_RECT_F ground { 
                MS_GROUNDWIDTH * i - groundOffset,
                GAMES_SIZE_H - MS_GROUNDHEIGHT,
                MS_GROUNDWIDTH * i + MS_GROUNDWIDTH - groundOffset, 
                GAMES_SIZE_H
            };
            Graphics::Instance().DrawBitmap(imgGround, ground);
        }
    }

    void GameState::DrawPipes()
    {
        // 绘制管子
        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            // 计算上下水管绘制位置
            D2D1_RECT_F topScreen, topImage, bottomScreen, bottomImage;
            if (i->pos.x - MS_PIPEWIDTH / 2 < 0.f)
            {
                topScreen.left = 0;
                topImage.left = MS_PIPEWIDTH / 2 - i->pos.x;
                bottomScreen.left = 0;
                bottomImage.left = MS_PIPEWIDTH / 2 - i->pos.x;
            }
            else
            {
                topScreen.left = i->pos.x - MS_PIPEWIDTH / 2;
                topImage.left = 0;
                bottomScreen.left = i->pos.x - MS_PIPEWIDTH / 2;
                bottomImage.left = 0;
            }

            if (i->pos.x + MS_PIPEWIDTH / 2 > GAMES_SIZE_W)
            {
                topScreen.right = GAMES_SIZE_W;
                topImage.right = MS_PIPEWIDTH - i->pos.x - MS_PIPEWIDTH / 2 + GAMES_SIZE_W;
                bottomScreen.right = GAMES_SIZE_W;
                bottomImage.right = MS_PIPEWIDTH - i->pos.x - MS_PIPEWIDTH / 2 + GAMES_SIZE_W;
            }
            else
            {
                topScreen.right = i->pos.x + MS_PIPEWIDTH / 2;
                topImage.right = MS_PIPEWIDTH;
                bottomScreen.right = i->pos.x + MS_PIPEWIDTH / 2;
                bottomImage.right = MS_PIPEWIDTH;
            }

            topScreen.top = 0;
            topScreen.bottom = i->pos.y - MS_PIPEGAPHEIGHT / 2.f;
            topImage.top = MS_PIPEHEIGHT - topScreen.bottom;
            topImage.bottom = MS_PIPEHEIGHT;

            bottomScreen.top = i->pos.y + MS_PIPEGAPHEIGHT / 2.f;
            bottomScreen.bottom = GAMES_SIZE_H - MS_GROUNDHEIGHT;
            bottomImage.top = 0;
            bottomImage.bottom = bottomScreen.bottom - bottomScreen.top;

            // 绘制上下水管
            Graphics::Instance().DrawBitmap(imgPipeTop, topScreen, topImage);
            Graphics::Instance().DrawBitmap(imgPipeBottom, bottomScreen, bottomImage);
        }
    }

    void GameState::DrawPath()
    {
        // 绘制鸟的路径
        for (auto i = birdPath.begin(); i != birdPath.end(); ++i)
        {
            int tPathColor = MS_PATHCOLOR;
            int alpha = 1.f;
            if (i->first > MS_PATHSTARTFADEOUTTIME)
                alpha = ((i->first - MS_PATHSTARTFADEOUTTIME) / (MS_PATHFADEOUTTIME - MS_PATHSTARTFADEOUTTIME));

            D2D1_ELLIPSE ellipse{ i->second.x, i->second.y, MS_PATHSIZE, MS_PATHSIZE };
            Graphics::Instance().FillEllipse(ellipse, tPathColor, alpha);
        }
    }

    void GameState::DrawBrid()
    {
        birds->draw();
    }

    void GameState::DrawCollisionBox()
    {
        D2D1_ELLIPSE ellipse{
            { birdPos.x, birdPos.y },
            MS_BIRDBOUNDINGCIRCLESIZE,
            MS_BIRDBOUNDINGCIRCLESIZE };

        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            // 计算上下水管绘制位置
            D2D1_RECT_F top, bottom;
            if (i->pos.x - MS_PIPEWIDTH / 2 < 0.f)
            {
                top.left = 0;
                bottom.left = 0;
            }
            else
            {
                top.left = i->pos.x - MS_PIPEWIDTH / 2;
                bottom.left = i->pos.x - MS_PIPEWIDTH / 2;
            }

            if (i->pos.x + MS_PIPEWIDTH / 2 > GAMES_SIZE_W)
            {
                top.right = GAMES_SIZE_W;
                bottom.right = GAMES_SIZE_W;
            }
            else
            {
                top.right = i->pos.x + MS_PIPEWIDTH / 2;
                bottom.right = i->pos.x + MS_PIPEWIDTH / 2;
            }

            top.top = 0;
            top.bottom = i->pos.y - MS_PIPEGAPHEIGHT / 2.f;

            bottom.top = i->pos.y + MS_PIPEGAPHEIGHT / 2.f;
            bottom.bottom = GAMES_SIZE_H - MS_GROUNDHEIGHT;

            Graphics::Instance().DrawRectangle(top, 0xff0000);
            Graphics::Instance().DrawRectangle(bottom, 0xff0000);
        }

        D2D1_POINT_2F p1{ 0, GAMES_SIZE_H - MS_GROUNDHEIGHT },
            p2{ GAMES_SIZE_W, GAMES_SIZE_H - MS_GROUNDHEIGHT };
        Graphics::Instance().DrawLine(p1, p2, 0xff0000);

        Graphics::Instance().DrawEllipse(ellipse, 0xff0000);
    }

    void GameState::FloatBird(double ElapsedTime)
    {
        static int count = 0, step = 1;
        static float time = 0.f;

        time += ElapsedTime;
        if (time > 0.05f)
        {
            time = 0.f;
            birdPos.y += step;
            birds->setPosition(
                birdPos.x - birdWidth / 2,
                birdPos.y - birdHeight / 2,
                birdPos.x + birdWidth / 2,
                birdPos.y + birdHeight / 2);

            if (++count >= 5)
            {
                step = -step;
                count = 0;
            }
        }
    }

    void GameState::UpdateBird(double ElapsedTime)
    {
        // 改变鸟的垂直位置
        birdPos.y += birdV * ElapsedTime;
        if (birdPos.y - MS_BIRDBOUNDINGCIRCLESIZE < 0.f) // 禁止越界
            birdPos.y = MS_BIRDBOUNDINGCIRCLESIZE;
        if (birdPos.y + MS_GROUNDHEIGHT > GAMES_SIZE_H)
            birdPos.y = GAMES_SIZE_H - MS_GROUNDHEIGHT;
        birdV += MS_GRAVITY * ElapsedTime;
        if (birdV >= MS_BIRDMAXV)  // 最大速度
            birdV = MS_BIRDMAXV;

        birds->setPosition(
            birdPos.x - birdWidth / 2, 
            birdPos.y - birdHeight / 2, 
            birdPos.x + birdWidth / 2, 
            birdPos.y + birdHeight / 2);

        // 计算鸟的旋转
        float tBirdTargetRotation = 0.f;
        if (birdV <= MS_BIRD_BEGIN_DROP)
            tBirdTargetRotation = -MS_BIRDMAXROTATION;
        else if (birdV > MS_BIRD_BEGIN_DROP && birdV < MS_BIRDDROPV)
            tBirdTargetRotation = birdV / MS_BIRDDROPV * (MS_BIRDMAXROTATION + MS_BIRDMINROTATION) - MS_BIRDMAXROTATION;
        else if (birdV >= MS_BIRDDROPV)
            tBirdTargetRotation = MS_BIRDMINROTATION;

        // 计算过渡旋转值(ease-out)
        birdRotation = (birdRotation * (MS_BIRDROTATIONWEIGHT - 1.f) + tBirdTargetRotation) / MS_BIRDROTATIONWEIGHT;
        birds->setRotation(birdRotation / 3.1415926f * 180, D2D1::Point2F(birdPos.x, birdPos.y));
    }

    void GameState::UpdatePipes(double ElapsedTime)
    {
        // 移动管子
        auto i = pipes.begin();
        while (i != pipes.end())
        {
            // 计算当前管子位置
            i->pos.x -= ElapsedTime * MS_STAGEMOVESPEED;

            // 移除越界管子
            if (i->pos.x + MS_PIPEWIDTH / 2.f < 0.f)
                i = pipes.erase(i);
            else
                ++i;
        }
    }

    void GameState::UpdateGround(double ElapsedTime)
    {
        // 移动地板
	    groundOffset = (float)fmod(groundOffset + ElapsedTime * MS_STAGEMOVESPEED, MS_GROUNDWIDTH);
    }

    void GameState::UpdatePathLogger(double ElapsedTime)
    {
        // 记录鸟的位置
        birdPathLoggerTime += ElapsedTime;
        if (birdPathLoggerTime > MS_PATHLOGTIME)
        {
            birdPath.push_back(std::pair<float, Point>(0.f, birdPos));

            birdPathLoggerTime = 0.f;
        }

        // 更新鸟的路径点
        auto i = birdPath.begin();
        while (i != birdPath.end())
        {
            i->first += ElapsedTime;
            i->second.x -= MS_STAGEMOVESPEED * ElapsedTime;

            if (i->first > MS_PATHFADEOUTTIME)
                i = birdPath.erase(i);
            else
                ++i;
        }
    }

    COLLISIONRESULT GameState::CheckCollision(int &PipeIndex)
    {
        PipeIndex = -1;
        D2D1_ELLIPSE ellipse{
            { birdPos.x, birdPos.y },
            MS_BIRDBOUNDINGCIRCLESIZE,
            MS_BIRDBOUNDINGCIRCLESIZE };

        // 检查与水管的碰撞
	    for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            // 计算上下水管绘制位置
            D2D1_RECT_F top, bottom;
            if (i->pos.x - MS_PIPEWIDTH / 2 < 0.f)
            {
                top.left = 0;
                bottom.left = 0;
            }
            else
            {
                top.left = i->pos.x - MS_PIPEWIDTH / 2;
                bottom.left = i->pos.x - MS_PIPEWIDTH / 2;
            }

            if (i->pos.x + MS_PIPEWIDTH / 2 > GAMES_SIZE_W)
            {
                top.right = GAMES_SIZE_W;
                bottom.right = GAMES_SIZE_W;
            }
            else
            {
                top.right = i->pos.x + MS_PIPEWIDTH / 2;
                bottom.right = i->pos.x + MS_PIPEWIDTH / 2;
            }

            top.top = 0;
            top.bottom = i->pos.y - MS_PIPEGAPHEIGHT / 2.f;

            bottom.top = i->pos.y + MS_PIPEGAPHEIGHT / 2.f;
            bottom.bottom = GAMES_SIZE_H - MS_GROUNDHEIGHT;

            // 进行碰撞检测
            if (Collision::OBBCircleHitTest(top, ellipse))
            {
                PipeIndex = i->Index;
                return COLLISIONRESULT_TOP_PIPE;
            }
            else if (Collision::OBBCircleHitTest(bottom, ellipse))
            {
                PipeIndex = i->Index;
                return COLLISIONRESULT_BOTTOM_PIPE;
            }
        }

        // 检查与地面的碰撞
        D2D1_POINT_2F p1 { 0, GAMES_SIZE_H - MS_GROUNDHEIGHT },
            p2 { GAMES_SIZE_W, GAMES_SIZE_H - MS_GROUNDHEIGHT };
        if (Collision::CircleLineHitTest(ellipse, p1, p2))
        {
            return COLLISIONRESULT_GROUND;
        }
        return COLLISIONRESULT_NONE;
    }

    void GameState::CheckScore()
    {
        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            if (birdPos.x > i->pos.x && i->Passed == false)
            {
                i->Passed = true;
                PassPipe();
            }
        }
    }

    void GameState::PassPipe()
    {
        score++;
    }

    void GameState::GetBirdState(float & horizontal, float & vertical, float & ground_distance)
    {
        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            if (i->pos.x + (MS_PIPEWIDTH >> 1) - birdPos.x < 0)
                continue;

            horizontal = i->pos.x + (MS_PIPEWIDTH >> 1) - birdPos.x;
            vertical = i->pos.y - birdPos.y;
            ground_distance = GAMES_SIZE_H - MS_GROUNDHEIGHT - birdPos.y;
            break;
        }
    }

}
