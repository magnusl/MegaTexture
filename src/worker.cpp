#include "worker.h"
#include "task.h"

namespace tasks {

IWorker::IWorker(Event & ev, size_t _id) : 
    nextToSteal(0),
    handle(0),
    id(_id),
    semaphore(0),
    pauseEvent(ev),
    terminate(false)
{
    ::InitializeCriticalSection(&cs);
}

/** startes executing the worker */
bool IWorker::init(IWorker ** _workers, int num)
{
    workers     = _workers;
    numWorkers  = num;

    handle = CreateThread(0, 0, IWorker::ThreadFunc, this, 0, nullptr);
    if (!handle) {
        return false;
    }
    return true;
}

DWORD IWorker::ThreadFunc(LPVOID arg)
{
    IWorker * ptr = (IWorker *) arg;
    if (arg != nullptr) {
        ptr->Body();
    }
    return 0;
}

/** tell the worker to resume working */
void IWorker::resume()
{
    /** increment the value of the semaphore to allow it to run */
    semaphore.Signal();
}

void IWorker::destroy()
{
    terminate = true;
    semaphore.Signal();
}

void IWorker::wait()
{
    WaitForSingleObject(handle, INFINITE);
}

/** executes the tasks */
void IWorker::Body()
{
    semaphore.Wait();
    for(;;) {
        // try to fetch a task from the queue
        TaskNode * task = fetch(id);
        if (!task) {
            // no local task available, steal one.
            task = steal();
        }
        if (task) {
            task->Execute( this );
        }
        if (terminate) {
            return;
        }
        if (!task) { /* no task found, should we pause? */
            if (pauseEvent.IsSignaled()) {
                /** wait until we are allowed to run */
                semaphore.Wait();
            }
            if (terminate) {
                return;
            }
        }
    }
}

void IWorker::addTask(TaskNode * task) 
{
    ::EnterCriticalSection( &cs );
    taskQueue.push_back( task );
    ::LeaveCriticalSection( &cs );
}

TaskNode * IWorker::fetch(int threadId)
{
    TaskNode * task = nullptr;
    ::EnterCriticalSection( &cs );
    if (!taskQueue.empty()) {
        for(auto iter = taskQueue.begin(); iter != taskQueue.end(); iter++) {
            int affinity = (*iter)->GetAffinity();
            // leave threads that can't be executed on this thread
            if ((affinity == -1) || (affinity == threadId)) {
                task = *iter;
                taskQueue.erase( iter );
                break;
            }
        }
    }
    ::LeaveCriticalSection( &cs );
    return task;
}

TaskNode * IWorker::steal()
{
    int start = nextToSteal;
    for(;;) {
        /** steal using a round-rubin fashion, try all workers and then return */
        if (numWorkers) {
            TaskNode * node = workers[nextToSteal]->fetch( id );
            nextToSteal = (nextToSteal + 1) % numWorkers;
            if (node) {
                return node;
            }
        }
        if (nextToSteal == start) {
            break;
        }
    }
    return nullptr;
}

} // namespace tasks