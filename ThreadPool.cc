#include "ThreadPool.H"


__nThreadBegin__

ThreadPool::
ThreadPool(int threadNum)
{
        _threadNum = threadNum;
        createThreads();
        _isRunning = true;
}

ThreadPool::
~ThreadPool()
{
        stop();
        for (std::deque<Task*>::iterator itor = _taskQue.begin();
             itor != _taskQue.end(); itor++) {
                delete *itor;
        }
        _taskQue.clear();
}

int
ThreadPool::
createThreads()
{
        pthread_mutex_init(&_mutex, NULL);
        pthread_cond_init(&_cond, NULL);

        _threads = (pthread_t *)malloc(sizeof(pthread_t) * _threadNum);

        for (int i = 0; i < _threadNum; i++) {
                pthread_create(&_threads[i], NULL, threadRunTask, this);
        }

        return 0;
}

int
ThreadPool::
addTask(Task *task)
{
        pthread_mutex_lock(&_mutex);

        _taskQue.push_back(task);
        int sz = _taskQue.size();

        pthread_mutex_unlock(&_mutex);

        // 发放信号
        pthread_cond_signal(&_cond);

        return sz;
}

void
ThreadPool::
stop()
{
        if (!_isRunning) {
                return;
        }

        _isRunning = false;
        pthread_cond_broadcast(&_cond);

        for (int i = 0; i < _threadNum; i++) {
                pthread_join(_threads[i], NULL);
        }

        free(_threads);
        _threads = nullptr;

        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);

        return;
}

int
ThreadPool::
size()
{
        pthread_mutex_lock(&_mutex);
        int sz = _taskQue.size();
        pthread_mutex_unlock(&_mutex);

        return sz;
}

Task*
ThreadPool::
take()
{
        Task *task = nullptr;

        while (!task) {
                pthread_mutex_lock(&_mutex);

                // 刚创建并末有线程加入则阻塞 addTask() 中会释放 _cond
                while (_taskQue.empty() && _isRunning) {
                        pthread_cond_wait(&_cond, &_mutex);
                }

                if (!_isRunning) {
                        // 线程池末运行态, stop()
                        pthread_mutex_unlock(&_mutex);
                        break;
                } else if (_taskQue.empty()) {
                        // 任务队列空
                        pthread_mutex_unlock(&_mutex);
                        continue;
                }

                assert(!_taskQue.empty());
                task = _taskQue.front();
                _taskQue.pop_front();

                pthread_mutex_unlock(&_mutex);
        }

        return task;
}

void*
ThreadPool::
threadRunTask(void *arg)
{
        pthread_t selfTid = pthread_self();
        ThreadPool *pool = (ThreadPool *)arg;

        while (pool->_isRunning) {
                Task *task = pool->take();

                if (!task) {
                        printf("No Task Left -> Thread %lu will exit now\n", selfTid);
                        break;
                }

                assert(task);
                task->execute();

        }

        return NULL;
}

__nThreadEnd__
