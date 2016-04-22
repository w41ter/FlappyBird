#include "main.h"
#include "MessageQueue.h"

namespace games
{

    MessageQueue::MessageQueue()
    {
    }


    MessageQueue::~MessageQueue()
    {
    }

    MessageQueue & MessageQueue::instance()
    {
        static MessageQueue queue;
        return queue;
    }

    void MessageQueue::SendMessage(Message msg)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(msg);
        }
        condvar_.notify_all();
    }

    MessageQueue::Message MessageQueue::GetMessage()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condvar_.wait(lock, [this] {return !queue_.empty();});
        assert(!queue_.empty());
        Message msg = queue_.front();
        queue_.pop();

        return msg;
    }

    bool MessageQueue::HasMessage()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size() != 0;
    }

    
}