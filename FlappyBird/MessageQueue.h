#pragma once

#include <functional>
#include <queue>
#include <condition_variable>

namespace games
{
    class MessageQueue
    {
        using Message = std::function<void(void)>;
    public:
        static MessageQueue &instance();

        void SendMessage(Message msg);
        Message GetMessage();
        bool HasMessage();

    private:
        MessageQueue();
        ~MessageQueue();

    private:
        mutable std::mutex mutex_;
        std::condition_variable condvar_;
        std::queue<Message> queue_;
    };

}