#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

template<typename Type>
class ThreadPool
{
public:
    using Thread = std::function<void(const Type&)>;

    ThreadPool(int num, Thread thread);
    ~ThreadPool();

    void commit(const Type &type);

public:
    std::queue<Type> que;
    std::mutex m;
    std::condition_variable cond_var;
    bool exit = false;
    int thread_num;
};

template<typename Type>
ThreadPool<Type>::ThreadPool(int num, Thread thread)
    : thread_num(num)
{
    while (num--)
    {
        std::thread worker([this, thread] {
            while (true) {
                std::unique_lock<std::mutex> lock(this->m);
                this->cond_var.wait(lock, [this] {return !que.empty() || exit;});
                if (exit) return;
                assert(!this->que.empty());
                Type param = que.front();
                que.pop();
                lock.unlock();
                thread(param);
                std::this_thread::yield();
            }
        });
        worker.detach();
    }
}

template<typename Type>
ThreadPool<Type>::~ThreadPool()
{
    exit = true;
    cond_var.notify_all();
}

template<typename Type>
void ThreadPool<Type>::commit(const Type &type)
{
    std::unique_lock<std::mutex> lock(this->m);
    que.push(type);
    lock.unlock();
    cond_var.notify_one();
}