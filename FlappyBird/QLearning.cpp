#include "main.h"
#include "QLearning.h"

#include <fstream>
#include <Windows.h>
#include <time.h>

#include "Ulit.h"
#include "constant.h"
#include "GameState.h"
#include "Application.h"

#define RangeRand(x, y) ((rand() % ((y) - (x) + 1)) + (x))  //取得[x,y]的随机整数

#define BUFSIZE 50
#define XGRID 6 // X方向离散化每格像素数
#define YGRID 6 // Y方向离散化每格像素数
#define GYGRID 40  // Y方向离地距离离散化每格像素数 

#define PIPE_GAP (MS_STAGEMOVESPEED * MS_PIPECREATETIME) // 管子间隔
#define MAX_X_OFFSET PIPE_GAP // X方向最大偏移量
#define MIN_X_OFFSET 0   // X方向最小偏移量

#define MAX_Y_OFFSET (0) // Y方向最大偏移量
#define MIN_Y_OFFSET (-MS_PIPEGAPHEIGHT)// Y方向最小偏移量

#define MAX_GY_OFFSET 40  // 距地距离最大偏移量
#define MIN_GY_OFFSET 0  // 距地距离最小偏移量
 
#define X_BIN (int((MAX_X_OFFSET - MIN_X_OFFSET) / XGRID))
#define Y_BIN (int((MAX_Y_OFFSET - MIN_Y_OFFSET) / YGRID) + 2)
#define GY_BIN (int((MAX_GY_OFFSET - MIN_GY_OFFSET) / GYGRID) + 1)
#define BIRD_STATE_NO 2 // 鸟的生命状态数 
#define ACTION_NO 2

int ind_x(int x)
{
    return max(0, min(X_BIN - 1, (x - MIN_X_OFFSET) / XGRID));
}

int ind_y(int y)
{
    if (y > MAX_Y_OFFSET)
        return Y_BIN - 1;
    else if (y < MIN_Y_OFFSET)
        return 0;
    return max(1, min(Y_BIN - 2, (y - MIN_Y_OFFSET) / YGRID));
}

int ind_gy(int gy)
{
    if (gy < GYGRID)
        return 0;
    return max(1, min(GY_BIN - 1, (gy - MIN_GY_OFFSET) / GYGRID));
}

QLearning::QLearning()
    : horizontal_distance(X_BIN)
    , vertical_distance(Y_BIN)
    , ground_max_distance(GY_BIN)
    , action_number(ACTION_NO)
    , matrix(nullptr)
    , discount(0.7f)
    , gramma(1.0f)
{
    configFileName = GetAppPath() + L"Resource\\data.txt";
    initData();
    createMatrix();
    loadMatrix();
}

QLearning::~QLearning()
{
    releaseMatrix();
}

void QLearning::initData()
{
    last_horizontal = 0;
    last_vertical = RangeRand(0, Y_BIN - 1);
    last_ground_distance = 1;
    last_action = 1;
}

void QLearning::createMatrix()
{
    matrix = new float ***[horizontal_distance];
    for (int i = 0; i < horizontal_distance; ++i)
    {
        matrix[i] = new float **[vertical_distance];
        for (int j = 0; j < vertical_distance; ++j)
        {
            matrix[i][j] = new float *[ground_max_distance];
            for (int m = 0; m < ground_max_distance; ++m)
                matrix[i][j][m] = new float[action_number];
        }
    }
}

void QLearning::releaseMatrix()
{
    for (int i = 0; i < horizontal_distance; ++i) {
        for (int j = 0; j < vertical_distance; ++j) {
            for (int m = 0; m < ground_max_distance; ++m) {
                delete []matrix[i][j][m];
            }
            delete []matrix[i][j];
        }
        delete []matrix[i];
    }
    delete []matrix;
    matrix = nullptr;
}

void QLearning::loadMatrix()
{
    std::wfstream file(configFileName, std::ios::in);
    if (file)
    {
        for (int i = 0; i < horizontal_distance; ++i)
            for (int j = 0; j < vertical_distance; ++j)
                for (int m = 0; m < ground_max_distance; ++m)
                    for (int n = 0; n < action_number; ++n)
                        file >> matrix[i][j][m][n];
    } 
    else
    {
        for (int i = 0; i < horizontal_distance; ++i)
            for (int j = 0; j < vertical_distance; ++j)
                for (int m = 0; m < ground_max_distance; ++m)
                    for (int n = 0; n < action_number; ++n)
                        matrix[i][j][m][n] = 0;
    }
}

void QLearning::saveMatirx()
{
    std::wfstream file(configFileName, std::ios::out);
    for (int i = 0; i < horizontal_distance; ++i)
    {
        for (int j = 0; j < vertical_distance; ++j)
            for (int m = 0; m < ground_max_distance; ++m)
                for (int n = 0; n < action_number; ++n)
                    file << matrix[i][j][m][n] << '\t';
        file << std::endl;
    }
}

int QLearning::findMax(float & value, int horizontal, int vertical, int ground_distance)
{
    int kk = 0;
    value = -1000000000;
    for (int k = 0; k < action_number; ++k) {
        int v = matrix[horizontal][vertical][ground_distance][k];
        if (value < v) {
            value = v;
            kk = k;
        }
    }
    return kk;
}

void QLearning::run(int horizontal, int vertical, int ground_distance, int life)
{
    int action = 0;
    float maximizeQ = 0;

    horizontal = ind_x(horizontal);
    vertical = ind_y(vertical);
    ground_distance = ind_gy(ground_distance);

    // Step1: find the best action a for current state s
    // In order to get a faster convergence, some state
    // space only have one action to take

    if (vertical == Y_BIN - 1)
    {
        // bird is higher than pipe center, action space 
        // just have "no click" action to take
        action = 0;  
    }
    else if (vertical == 0 || ground_distance == 0)
    {
        // bird is much lower than pipe center or it's 
        // close to ground, action space just have "click"
        // action to take
        action = 1;  
    }
    else
    {
        // otherwise,take the action to maximize Q(s',a)
        action = findMax(maximizeQ, horizontal, vertical, ground_distance);
    }

    // Step2: update the Q(s,a)
    // if alive, reward R = 1
    // otherwise, reward R = -1000

    float &value = matrix[last_horizontal][last_vertical][last_ground_distance][last_action];
    if (life == 1) 
    {
        value += discount * (1 + gramma * maximizeQ - value);
    }
    else
    {
        value += discount * (-1000 + gramma * maximizeQ - value);
    }

    // Step3: take the action a
    if (action)
    {
        //keybd_event(VK_SPACE, 0, 0, 0);
        //keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
        games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYDOWN, VK_SPACE);
        games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYRUP, VK_SPACE);
    }

    // Step4: update s with s'
    last_horizontal = horizontal;
    last_vertical = vertical;
    last_ground_distance = ground_distance;
    last_action = action;

    // if died, need save Q(s, a)
    // and restart the game
    if (!life)
    {
        initData();
        saveMatirx();
    }
}

void QLearning::die()
{
    games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYDOWN, VK_SPACE);
    games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYRUP, VK_SPACE);
    games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYDOWN, VK_SPACE);
    games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYRUP, VK_SPACE);
    games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYDOWN, VK_SPACE);
    games::GameState::instance().OnKeywordEvent(games::KeywordEvent::KEYRUP, VK_SPACE);
}
