#ifndef THREADPOOL_H
#define THREADPOOL_H

// 标准库
#include <iostream>
#include <memory>
#include <vector>
#include <deque>
#include <string>

// 线程
#include <pthread.h>

// c函数
#include <stdlib.h>
#include <assert.h>

// 只是为了代码折叠能左靠齐
#define __nThreadBegin__ namespace nThreadPool {
#define __nThreadEnd__ }


__nThreadBegin__

// 任务需要继承 Task 类
class Task {
public:
        Task() {};
        virtual ~Task() {};

        // 纯虚函数
        virtual void execute() = 0;
};

class ThreadPool {
public:
        ThreadPool(int threadNum = 10);
        ~ThreadPool();

        // 禁止复制
        ThreadPool(ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&) = delete;

        // 任务操作
        int addTask(Task *task);
        void stop();
        int size();
        Task* take();

private:
        int createThreads();
        static void* threadRunTask(void *arg);

        // 标识数据
        bool _isRunning;
        int _threadNum;

        pthread_t* _threads;

        // 任务队列
        std::deque<Task*> _taskQue;
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
};

__nThreadEnd__

#endif /* THREADPOOL_H */
