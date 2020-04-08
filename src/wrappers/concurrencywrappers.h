#include <semaphore.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <pthread.h>

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
void Pthread_join(pthread_t thread, void **retval);
void Pthread_mutex_lock(pthread_mutex_t *mutex);
void Pthread_mutex_unlock(pthread_mutex_t *mutex);
void Pthread_mutex_destroy(pthread_mutex_t *mutex);
void Pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);

void Pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr);
void Pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
void Pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
void Pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
void Pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
void Pthread_rwlockattr_setkind_np(pthread_rwlockattr_t *attr, int pref);

void Sem_init(sem_t *sem, int pshared, unsigned int value);
// need more for sem

int Eventfd(unsigned int initval, int flags);
void Eventfd_read(int fd, eventfd_t *value);
void Eventfd_write(int fd, eventfd_t value);

int Timerfd_create(int clockid, int flags);
void Timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);