#pragma once

class QLearning
{
    using StateMatrix = float ****;
public:
    QLearning();
    ~QLearning();

    void run(
        int horizontal,
        int vertical,
        int ground_distance,
        int life);

    void die();
private:
    void initData();
    void createMatrix();
    void releaseMatrix();
    void loadMatrix();
    void saveMatirx();

    int findMax(
        float &value,
        int horizontal,
        int vertical,
        int ground_distance);

private:
    std::wstring configFileName;

    const float discount;
    const float gramma;

    StateMatrix matrix;

    // 
    const int horizontal_distance;
    const int vertical_distance;
    const int ground_max_distance;
    const int action_number;

    int last_horizontal;
    int last_vertical;
    int last_ground_distance;
    int last_action;
};

