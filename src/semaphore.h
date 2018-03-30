#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <Windows.h>

namespace tasks {

class Semaphore
{
public:
    Semaphore(int initial)
    {
        m_hSemaphore = CreateSemaphore(NULL, initial, 10000, NULL);
    }

    ~Semaphore()
    {
        CloseHandle(m_hSemaphore);
    }

    void Wait()
    {
        WaitForSingleObject(m_hSemaphore, INFINITE);
    }

    void Signal()
    {
        ReleaseSemaphore(m_hSemaphore, 1, NULL);
    }

protected:
    HANDLE m_hSemaphore;
};

} // namespace tasks

#endif