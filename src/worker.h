#ifndef WORKER_H
#define WORKER_H

#include <Windows.h>
#include <list>
#include "event.h"
#include "semaphore.h"
#include "task.h"

namespace tasks {

class IWorker
{
public:
    IWorker(Event &, size_t id);
    ~IWorker();

    bool init(IWorker ** workers, int num);
    void pause();
    void resume();
    void destroy();
    void wait();

    /** adds a task to the work queue */
    virtual void addTask(TaskNode *);
    /** steals a task from the work queue */
    virtual TaskNode * fetch(int);

    static DWORD CALLBACK ThreadFunc(LPVOID);

    int GetId() { return id; }

protected:
    void Body();
    TaskNode * steal(void);

    bool                    terminate;
    Semaphore               semaphore;
    Event &                 pauseEvent;
    CRITICAL_SECTION        cs;
    std::list<TaskNode*>    taskQueue;
    IWorker **              workers;
    int                     numWorkers;
    int                     id;
    int                     nextToSteal;
    HANDLE                  handle;
};


} // namespace tasks

#endif