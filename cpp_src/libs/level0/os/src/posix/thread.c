
#include "core/core.h"
#include "core/logger.h"
#include "os/thread.h"
//#include "../Interfaces/IMemoryManager.h"

#include <unistd.h>
#include <sys/sysctl.h>

EXTERN_C bool Os_MutexCreate(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  return pthread_mutex_init(mutex, NULL) == 0;
}

EXTERN_C void Os_MutexDestroy(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  pthread_mutex_destroy(mutex);
}

EXTERN_C void Os_MutexAcquire(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  pthread_mutex_lock(mutex);

}

EXTERN_C void Os_MutexRelease(Os_Mutex_t *mutex) {
  ASSERT(mutex);
  pthread_mutex_unlock(mutex);
}

EXTERN_C bool Os_ConditionalVariableCreate(Os_ConditionalVariable_t *cd) {
  ASSERT(cd);
  return pthread_cond_init(cd, NULL) == 0;
}

EXTERN_C void Os_ConditionalVariableDestroy(Os_ConditionalVariable_t *cd) {
  ASSERT(cd);
  pthread_cond_destroy(cd);
}

EXTERN_C void Os_ConditionalVariableWait(Os_ConditionalVariable_t *cd, Os_Mutex_t *mutex, uint64_t waitms) {
  ASSERT(cd);
  ASSERT(mutex);

  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = (long) waitms * 1000;

  pthread_mutex_t *mutexHandle = mutex;
  pthread_cond_timedwait(cd, mutexHandle, &ts);

}

EXTERN_C void Os_ConditionalVariableSet(Os_ConditionalVariable_t *cd) {
  ASSERT(cd);
  pthread_cond_signal(cd);
}

EXTERN_C bool Os_ThreadCreate(Os_Thread_t *thread, JobFunction_t func, void *data) {
  ASSERT(thread);

  return pthread_create(thread, NULL, func, data) == 0;
}

EXTERN_C void Os_ThreadDestroy(Os_Thread_t *thread) {
  ASSERT(thread);

  // should this be detach rather than join?
  pthread_join(*thread, NULL);
}

EXTERN_C void Os_ThreadJoin(Os_Thread_t *thread) {
  ASSERT(thread);
  pthread_join(*thread, NULL);
}

EXTERN_C void Os_Sleep(uint64_t waitms) {
  usleep((useconds_t) waitms * 1000);
}

EXTERN_C uint32_t Os_CPUCoreCount(void) {
  size_t len;
  unsigned int ncpu;
  len = sizeof(ncpu);
  sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  return (uint32_t) ncpu;
}
