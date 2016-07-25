#include "ThreadPool.H"

#include <unistd.h>

class TaskDemo final
        : public nThreadPool::Task
{
public:
        TaskDemo() {};

        virtual void execute() override {
                printf("Task Thread: %lu execute()\n", pthread_self());
                sleep(1);
                return;
        }
};

int
main(int argc, char *argv[])
{
        TaskDemo myTask;
        nThreadPool::ThreadPool pool(5);

        for (int i = 0; i < 15; i++) {
                pool.addTask(&myTask);
        }

        while(true) {
                if (pool.size() == 0) {
                        pool.stop();
                        return 0;
                }
                sleep(2);
        }

        return 0;
}
