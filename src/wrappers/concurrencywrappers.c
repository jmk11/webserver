#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "concurrencywrappers.h"
//#include "constants.h"
#include "wrappers.h"

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
    int success = pthread_create(thread, attr, start_routine, arg);
    if (success != 0) {
        error(0, success, "Error: Can't create thread");
        // It seems clearer to call exit() myself rather than with error, that's why I passed 0 to error
        // to make error()'s performance closer to perror. Because I'm only using error
        // instead of perror because it seems pthread_create returns the error number
        // rather than putting it in errno
        exit(THREADERROR);
    }
}


void Sem_init(sem_t *sem, int pshared, unsigned int value)
{
    int success = sem_init(sem, pshared, value);
    if (success != 0) {
        perror("Error: Can't initialise semaphore");
        exit(SEMAPHOREERROR);
    }
}

void Pthread_join(pthread_t thread, void **retval)
{
    int success = pthread_join(thread, retval);
    if (success != 0) {
        error(0, success, "Error: Can't join to thread");
        exit(THREADERROR);
    }
}

void Pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int success = pthread_mutex_lock(mutex);
    if (success != 0) {
        error(0, success, "Error: Can't lock mutex");
        exit(THREADERROR);
    }
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    int success = pthread_mutex_unlock(mutex);
    if (success != 0) {
        error(0, success, "Error: Can't unlock mutex");
        exit(THREADERROR);
    }
}

void Pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    int success = pthread_mutex_destroy(mutex);
    if (success != 0) {
        error(0, success, "Error: Can't destroy mutex");
        exit(THREADERROR);
    }
}

void Pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr)
{
    int success = pthread_mutex_init(mutex, attr);
    if (success != 0) {
        error(0, success, "Error: Can't initialise mutex");
        exit(THREADERROR);
    }
}

void Pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr)
{
    int success = pthread_rwlock_init(rwlock, attr);
    if (success != 0) {
        error(0, success, "Error: Can't initialise rwlock");
        exit(THREADERROR);
    }
}

void Pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    int success = pthread_rwlock_destroy(rwlock);
    if (success != 0) {
        error(0, success, "Error: Can't destroy rwlock");
        exit(THREADERROR);
    }
}

void Pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    int success = pthread_rwlock_rdlock(rwlock);
    if (success != 0) {
        error(0, success, "Error: Can't readlock rwlock");
        exit(THREADERROR);
    }
}

void Pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    int success = pthread_rwlock_wrlock(rwlock);
    if (success != 0) {
        error(0, success, "Error: Can't writelock rwlock");
        exit(THREADERROR);
    }
}

// what does this actually do its weird
void Pthread_rwlockattr_setkind_np(pthread_rwlockattr_t *attr, int pref)
{
    int success = pthread_rwlockattr_setkind_np(attr, pref);
    if (success != 0) {
        error(0, success, "Error: Can't set attribute on rwlock");
        exit(THREADERROR);
    }
}

// !! CHECK WHAT UNLOCK ACTUALLY RETURNS
void Pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    int success = pthread_rwlock_unlock(rwlock);
    if (success != 0) {
        error(0, success, "Error: Can't unlock rwlock");
        exit(THREADERROR);
    }
}

int Eventfd(unsigned int initval, int flags)
{
    int fd = eventfd(initval, flags);
    if (fd == -1) {
        perror("Error: Can't create eventfd");
        exit(EVENTFDERROR);
    }
    return fd;
}

void Eventfd_read(int fd, eventfd_t *value)
{
    int success = eventfd_read(fd, value);
    if (success != 0) {
        fprintf(stderr, "Error: while reading eventfd\n");
        exit(EVENTFDERROR);
    }
}

void Eventfd_write(int fd, eventfd_t value)
{
    int success = eventfd_write(fd, value);
    if (success != 0) {
        fprintf(stderr, "Error: while writing to eventfd\n");
        exit(EVENTFDERROR);
    }
}

int Timerfd_create(int clockid, int flags)
{
    int fd = timerfd_create(clockid, flags);
    if (fd < 0) {
        perror("Error: in creating timerfd");
        exit(TIMERFDERROR);
    }
    return fd;
}

void Timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value)
{
    int success = timerfd_settime(fd, flags, new_value, old_value);
    if (success != 0) {
        perror("Error: in setting timerfd");
        exit(TIMERFDERROR);
    }
}

/*
void Timerfd_gettime(int fd, struct itimerspec *curr_value)
{
    int success = sigaction(signum, act, oldact);
    if (success != 0) {
        perror("Error: in setting sighandler");
        exit(SIGHANDLERERROR);
    }
}*/
