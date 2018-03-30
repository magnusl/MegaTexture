#ifndef _EVENT_H_
#define _EVENT_H_

#include <Windows.h>

namespace tasks {

class Event
{
public:
    Event()
    {
        m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    ~Event()
    {
        CloseHandle(m_hEvent);
    }

    void Signal(void)
    {
        SetEvent(m_hEvent);
    }

    bool IsSignaled(void)
    {
        return (WaitForSingleObject(m_hEvent, 0) == WAIT_OBJECT_0);
    }

    void Reset()
    {
        ResetEvent(m_hEvent);
    }

    void Wait()
    {
        WaitForSingleObject(m_hEvent, INFINITE);
    }

public:
    HANDLE m_hEvent;
};

} // namespace tasks

#endif