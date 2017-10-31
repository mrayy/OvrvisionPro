

#ifndef ___WinMutex___
#define ___WinMutex___

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class  WinMutex
{
	CRITICAL_SECTION m_mutex;
public:
	WinMutex();
	virtual~WinMutex();

	virtual void lock();
	virtual bool tryLock();
	virtual void unlock();
};


class ScopedLock
{
private:
	ScopedLock(const ScopedLock&){}
	ScopedLock& operator=(const ScopedLock&){}
protected:
	WinMutex* m_mutex;
public:
	explicit ScopedLock(WinMutex* m) :m_mutex(m)
	{
		m->lock();
	}
	~ScopedLock()
	{
		m_mutex->unlock();
	}
};


#endif




