#ifndef TASK_H
#define TASK_H

#include "event.h"
#include <vector>

namespace tasks {

class IWorker;

class TaskNode
{
public:
    TaskNode();

    /** returns the affinity */
    int GetAffinity(void) const     { return m_Affinity; }
    void SetAffinity(int affinity)  { m_Affinity = affinity; }

    /** should be implemented by derived classes */
    virtual void Compute() = 0;
    /** executes the Task in the context of the worker */
    void Execute(IWorker *);
    /** adds a dependecy to this task */
    void AddDependency(TaskNode *);
    /** resets the join count */
    void ResetJoin(int extra = 0) { this->m_JoinCount = this->m_NumDeps + extra; }
protected:

    virtual void                initial() {}
    /** the tasks that depends on this one */
    std::vector<TaskNode *>     m_Successors;
    unsigned int                m_JoinCount;
    unsigned int                m_NumDeps;
    int                         m_Affinity;
    bool                        m_InitialCalled;
};

/** represents a graph */
class TaskGroup
{
public:
    TaskGroup(size_t numWorkers);

    void SetGraph(TaskNode ** tasks, size_t numTasks);
    /** start execuring the task group */
    void Start(void);
    /** wait for the tasks to stop */
    void Wait(void);
    /** cleanup */
    void Stop(bool wait = true);
protected:
    class CompletionTask : public TaskNode
    {
    public:
        CompletionTask(Event & ev) : m_ev(ev)
        {
        }
        void Compute()
        {
            m_ev.Signal();
        }
    protected:
        Event & m_ev;
    };

    Event                   m_pauseEvent;
    Event                   m_completionEvent;
    TaskNode **             m_tasks;
    size_t                  m_numTasks;
    std::vector<IWorker *>  m_workers;
    CompletionTask          m_completeTask;
};

} // namespace tasks

#endif
