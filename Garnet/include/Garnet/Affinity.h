#ifndef GARNET__AFFINITY__H__
#define GARNET__AFFINITY__H__

#include <cstdint>

/**
 *
 * @param[in] num  A number of threads to be assigned to the current process.
 */
void  setThreadAffinity(int num);

/**
 *
 */
void  setCPUAffinity(uint32_t mask);

/**
 *
 */
void  setNumCPUs(int num);

/**
 *  Returns the number of logical CPUs on this computer.
 */
uint32_t  getNumCPUs();

#endif//GARNET__AFFINITY__H__

