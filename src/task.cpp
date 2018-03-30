#include "task.h"
#include "worker.h"
#include <algorithm>

namespace tasks {

TaskNode::TaskNode() : m_JoinCount(0), m_InitialCalled(false), m_Affinity(-1), m_NumDeps(0)
{

}

void TaskNode::AddDependency(TaskNode * task)
{
    if (task) {
        task->m_Successors.push_back(this);
        ++m_NumDeps;
    }
}

/**
 * Executes the task in the context of the worker.
 */
void TaskNode::Execute(IWorker * worker)
{
    /** initialize the tasks if needed */
    if (!m_InitialCalled) {
        initial();
        m_InitialCalled = true;
    }
    /** let the subclass perform the actual computation */
    Compute();
    /** notify any successors */
    std::for_each(m_Successors.begin(), m_Successors.end(), [worker](TaskNode * node) {
        if (::InterlockedDecrement(&node->m_JoinCount) == 0) {
            worker->addTask(node);
        }
    });
}

TaskGroup::TaskGroup(size_t numWorkers) 
    : m_completeTask(m_completionEvent)
{
    m_pauseEvent.Signal();
    for(size_t i = 0; i < numWorkers; i++) {
        this->m_workers.push_back( new IWorker(m_pauseEvent, i) );
    }

    std::for_each(m_workers.begin(), m_workers.end(), [this](IWorker * worker) {
        worker->init(&this->m_workers[0], this->m_workers.size());
    });
}

void TaskGroup::SetGraph(TaskNode ** tasks, size_t numTasks)
{
    for(size_t i = 0; i < numTasks; i++) {
        m_completeTask.AddDependency( tasks[i] );
    }
    m_numTasks  = numTasks;
    m_tasks     = tasks;
}

void TaskGroup::Start()
{
    m_completionEvent.Reset();
    m_pauseEvent.Reset();
    // add the task to the work worker
    int affinity = m_tasks[0]->GetAffinity();

    this->m_completeTask.ResetJoin();
    // reset join count
    for (size_t i = 0; i < m_numTasks; i++) {
        m_tasks[i]->ResetJoin();
    }
    // tell all workers to start
    std::for_each(this->m_workers.begin(), m_workers.end(), [this](IWorker * worker) {
        worker->resume();
    });

    // adding the task must be the last we do!
    m_workers[affinity == -1 ? 0 : affinity]->addTask(m_tasks[0]);
}

void TaskGroup::Wait()
{
    /** wait for the completion event to be set */
    m_completionEvent.Wait();
    /** tell the workers to pause */
    m_pauseEvent.Signal();
}

void TaskGroup::Stop(bool wait)
{
    // first tell the threads to shut down.
    std::for_each(this->m_workers.begin(), m_workers.end(), [this](IWorker * worker) {
        worker->destroy();
    });
    if (wait) {
        // wait for them to terminate
        std::for_each(this->m_workers.begin(), m_workers.end(), [this](IWorker * worker) {
            worker->wait();
        });
    }
}

} // namespace tasks
