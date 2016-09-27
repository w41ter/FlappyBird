#pragma once

namespace games 
{
    // TODO: 改变计时器 update 方式，使得与 frame 脱离关系
    // 
    struct Counter {
        int id;
        int priority;
        double use;
        double need;
        std::function<void(void)> selector;
    };

    class TimeCounter
    {
    public:
        static TimeCounter &instance();

        int Schedule(int priority, double need, std::function<void(void)> selector);
        void Release(int id);

        void Update();
        double getUse() const;
        double getTick() const;
    private:
        TimeCounter();
        ~TimeCounter();

        void RemoveReleaseTimeCounter();
        void Dispatch(double use);
        double Query();

        int AssignNewID();
    private:
        LARGE_INTEGER litmp;
        LONGLONG begin, end;

        double dfFreq;
        double use;

        std::list<Counter> selectors;
        std::queue<int> removeIDs;
    };

}