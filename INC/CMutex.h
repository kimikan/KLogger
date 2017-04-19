
/*
 * This file is for Mutex c++ wrapper.
 */

#ifndef _CMUTEX_H_
#define _CMUTEX_H_

class IMutexImpl;

class CMutex
{
public:
	CMutex();

	~CMutex();

public:
	void lock();

	void unLock();

private:

	IMutexImpl	*_pMutex;
};

#endif
