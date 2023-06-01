#ifndef _THREAD_UTIL_H
#define _THREAD_UTIL_H

#include <processthreadsapi.h>

class ThreadUtil
{
public:
	static unsigned long currentThreadId() {
		return ::GetCurrentThreadId();
	}

};


#endif