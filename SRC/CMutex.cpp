
/*
 * Mutex implementation file.
 * Written by , 2014/9/19
 */
#include <iostream>

#include "../INC/CMutex.h"
#include "Utility.h"

/*
 * Base class for Mutext implementation.
 * In fact, in real Mutex implementation, For debugging purpose,
 * The mutex implementation should be more complicated. 
 */
class IMutexImpl
{
public:

	virtual ~IMutexImpl();

public:
	virtual void lock() = 0;

	virtual void unLock() = 0;
};

IMutexImpl::~IMutexImpl()
{}

#ifdef WINDOWS

#include <Windows.h>

/*
 * Windows mutex implementation. 
 * TODO: for Linux(Portable request).
 */
class MutexWindowsImpl : public IMutexImpl
{
public:
    MutexWindowsImpl() : _hMutex(NULL)
    {
        _hMutex = ::CreateMutex(NULL, FALSE, NULL);
        if(!_hMutex)
        {
          DBGPRINTF("Create Mutex failed!\n");
        }
    }

    ~MutexWindowsImpl()
    {
        if(_hMutex != NULL)
        {
            ::CloseHandle(_hMutex);
            _hMutex = NULL;
        }
    }

    void lock()
    {
        CHECK_POINTER(_hMutex);
        ::WaitForSingleObject(_hMutex, INFINITE);
    }

    void unLock()
    {
        CHECK_POINTER(_hMutex);
        ::ReleaseMutex(_hMutex);
    }

private:

    HANDLE	_hMutex;
};

#else

#include <pthread.h>

class MutexLinuxImpl : public IMutexImpl
{
public:

    MutexLinuxImpl()
    {
        pthread_mutex_init(&_mutex, NULL);
    }

    ~MutexLinuxImpl()
    {
        pthread_mutex_destroy(&_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unLock()
    {
        pthread_mutex_unlock(&_mutex);
    }

private:
    pthread_mutex_t _mutex;

};

#endif

/*
 * Implementation for Mutex
 */
CMutex::CMutex() : _pMutex(NULL)
{
#ifdef WINDOWS
    _pMutex = new MutexWindowsImpl();
#endif

#ifdef LINUX
    _pMutex = new MutexLinuxImpl();
#endif

    if (NULL == _pMutex)
    {
        DBGPRINTF(" No supported Mutex implementation!\n");
    }
}

CMutex::~CMutex()
{
    if (_pMutex != NULL)
    {
        delete _pMutex;
        _pMutex = NULL;
    }
}

void CMutex::lock()
{
    CHECK_POINTER(_pMutex);
    _pMutex->lock();
}

void CMutex::unLock()
{
    CHECK_POINTER(_pMutex);
    _pMutex->unLock();
}
