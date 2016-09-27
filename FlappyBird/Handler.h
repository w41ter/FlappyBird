#pragma once

#include <memory>
#include <thread>

#include "MessageQueue.h"

namespace games
{
    template<typename T>
    class Handler
    {
        using EventChild = std::function<T(void)>;
        using EventMain = std::function<void(T)>;
    public:
        static std::shared_ptr<Handler> create()
        {
            return std::shared_ptr<Handler>(new Handler());
        }

        static void run(std::shared_ptr<Handler> handler)
        {
            if (!handler->child_)
                return;

            // 因为需要 detach，所以这里接管对象
            std::thread thread([handler] {
                auto result = handler->child_();
                if (handler->main_)
                {
                    MessageQueue::instance().SendMessage(
                        std::bind(handler->main_, result));
                }
            });
            thread.detach();
        }

        Handler &child(EventChild e)
        {
            child_ = e;
            return *this;
        }

        Handler &main(EventMain e)
        {
            main_ = e;
            return *this;
        }

        Handler::~Handler()
        {
        }
    protected:
        Handler::Handler()
        {
        }

    private:
        EventChild child_;
        EventMain main_;
    };

    template<>
    class Handler<void>
    {
        using EventChild = std::function<void(void)>;
        using EventMain = std::function<void(void)>;
    public:
        static std::shared_ptr<Handler> create()
        {
            return std::shared_ptr<Handler>(new Handler());
        }

        static void run(std::shared_ptr<Handler> handler)
        {
            if (!handler->child_)
                return;

            // 因为需要 detach，所以这里接管对象
            std::thread thread([handler] {
                handler->child_();
                if (handler->main_)
                {
                    MessageQueue::instance().SendMessage(handler->main_);
                }
            });
            thread.detach();
        }

        Handler &child(EventChild e)
        {
            child_ = e;
            return *this;
        }

        Handler &main(EventMain e)
        {
            main_ = e;
            return *this;
        }

        Handler::~Handler()
        {
        }

    protected:
        Handler::Handler()
        {
        }

    private:
        EventChild child_;
        EventMain main_;
    };
}