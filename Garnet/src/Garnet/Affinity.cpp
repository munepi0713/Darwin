

#include <Garnet/Affinity.h>

#if defined(_WIN32)
#include <Windows.h>
#include <process.h>
#include <stdio.h>

void  setThreadAffinity(int num)
{
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;
    if ( GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask) ) {
        // Count up the number of processes available for our app.
        int n = 0;
        for (int i = 0; i < sizeof(DWORD_PTR) * 4; i++) {
            if ( (systemAffinityMask & ((DWORD_PTR)1 << i)) ) n++;
        }

        // Limit max CPUs by user configuration.
        DWORD_PTR newAffinityMask = 0;
        if ( num < n ) {
            int m = 0;
            for (int i = 0; i < sizeof(DWORD_PTR) * 4; i++) {
                if ( (systemAffinityMask & ((DWORD_PTR)1 << i)) ) {
                    newAffinityMask |= ((DWORD_PTR)1 << i);
                    m++;
                    if ( m == num ) break;
                }
            }
            newAffinityMask <<= (n - num);
        }
        else {
            newAffinityMask = systemAffinityMask;
        }

        // Set new affinity mask.
        SetThreadAffinityMask(GetCurrentThread(), newAffinityMask);
    }
}

void  setCPUAffinity(uint32_t mask)
{
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;

	// Get system mask.
    GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask);
    // Set new affinity mask.
    SetProcessAffinityMask(GetCurrentProcess(), mask & systemAffinityMask);
}

void  setNumCPUs(int num)
{
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;
    if ( GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask) ) {
        // Count up the number of processes available for our app.
        int n = 0;
        for (int i = 0; i < sizeof(DWORD_PTR) * 4; i++) {
            if ( (systemAffinityMask & ((DWORD_PTR)1 << i)) ) n++;
        }

        // Limit max CPUs by user configuration.
        DWORD_PTR newAffinityMask = 0;
        if ( num < n ) {
            int m = 0;
            for (int i = 0; i < sizeof(DWORD_PTR) * 4; i++) {
                if ( (systemAffinityMask & ((DWORD_PTR)1 << i)) ) {
                    newAffinityMask |= ((DWORD_PTR)1 << i);
                    m++;
                    if ( m == num ) break;
                }
            }
            newAffinityMask <<= (n - num);
        }
        else {
            newAffinityMask = systemAffinityMask;
        }

        // Set new affinity mask.
        SetProcessAffinityMask(GetCurrentProcess(), newAffinityMask);
    }
}

uint32_t  getNumCPUs()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);	
	return static_cast<int>(info.dwNumberOfProcessors);
}

#else

#include <pthread.h>

void  setThreadAffinity(int num)
{
}

void  setNumCPUs(int num)
{
}

#endif

