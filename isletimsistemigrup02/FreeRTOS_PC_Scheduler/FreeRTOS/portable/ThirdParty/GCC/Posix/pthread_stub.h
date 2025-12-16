/* Windows'ta pthread.h stub - FreeRTOS POSIX portu için */
/* Bu dosya Windows'ta pthread.h olmadığında kullanılır */
#ifndef PTHREAD_STUB_H
#define PTHREAD_STUB_H

#ifdef _WIN32
#ifndef HAVE_PTHREAD_H

#include <windows.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>  /* memset için */
#include <errno.h>   /* errno, ETIMEDOUT vb. için */

/* errno sabitleri (Windows'ta birebir aynı olmak zorunda değil) */
#ifndef ETIMEDOUT
#define ETIMEDOUT 110
#endif

#ifndef EOWNERDEAD
#define EOWNERDEAD 130
#endif

/* clock_gettime için */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

/* Windows'ta clock_t tanımı */
#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

/* pthread tipleri */
typedef void* pthread_t;

typedef struct {
    int dummy;
} pthread_attr_t;

typedef struct {
    CRITICAL_SECTION cs;
} pthread_mutex_t;

/* Windows'ta condition variable - manuel implementasyon */
typedef struct {
    CRITICAL_SECTION waiters;
    HANDLE events[2];
    unsigned int waiters_count;
} pthread_cond_t;

typedef struct {
    int dummy;
} pthread_mutexattr_t;

typedef struct {
    int dummy;
} pthread_condattr_t;

typedef struct {
    int state;
} pthread_once_t;

typedef DWORD pthread_key_t;

/* pthread_once init */
#define PTHREAD_ONCE_INIT {0}

/* Signal constants */
#ifndef SIGUSR1
#define SIGUSR1 10
#endif
#ifndef SIGUSR2
#define SIGUSR2 12
#endif
#ifndef SIGALRM
#define SIGALRM 14
#endif
#ifndef SIG_BLOCK
#define SIG_BLOCK 0
#endif
#ifndef SIG_UNBLOCK
#define SIG_UNBLOCK 1
#endif
#ifndef SIG_SETMASK
#define SIG_SETMASK 2
#endif

/* Mutex initializer (tam POSIX uyumlu değil, ama compile için yeterli) */
#define PTHREAD_MUTEX_INITIALIZER { { NULL, 0, 0, NULL, NULL, 0 } }

/* Condition initializer (tam POSIX uyumlu değil, ama compile için yeterli) */
#define PTHREAD_COND_INITIALIZER { { NULL, 0, 0, NULL, NULL, 0 }, { NULL, NULL }, 0 }

/* ---------------- pthread API ---------------- */

static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                                 void *(*start_routine)(void *), void *arg)
{
    (void)attr;
    HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start_routine, arg, 0, NULL);
    if (h == NULL) return -1;
    *thread = (pthread_t)h;
    return 0;
}

static inline int pthread_join(pthread_t thread, void **retval)
{
    (void)retval;
    WaitForSingleObject((HANDLE)thread, INFINITE);
    CloseHandle((HANDLE)thread);
    return 0;
}

static inline pthread_t pthread_self(void)
{
    return (pthread_t)GetCurrentThread();
}

static inline int pthread_equal(pthread_t t1, pthread_t t2)
{
    return (t1 == t2) ? 1 : 0;
}

/* mutexattr stub'ları */
static inline int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    (void)attr;
    return 0;
}

static inline int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    (void)attr;
    return 0;
}

static inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    (void)attr;
    InitializeCriticalSection(&mutex->cs);
    return 0;
}

static inline int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    DeleteCriticalSection(&mutex->cs);
    return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    EnterCriticalSection(&mutex->cs);
    return 0;
}

static inline int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    LeaveCriticalSection(&mutex->cs);
    return 0;
}

static inline int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    (void)attr;
    cond->waiters_count = 0;
    cond->events[0] = CreateEvent(NULL, FALSE, FALSE, NULL); /* auto-reset */
    cond->events[1] = CreateEvent(NULL, TRUE,  FALSE, NULL); /* manual-reset */
    InitializeCriticalSection(&cond->waiters);
    return 0;
}

static inline int pthread_cond_destroy(pthread_cond_t *cond)
{
    CloseHandle(cond->events[0]);
    CloseHandle(cond->events[1]);
    DeleteCriticalSection(&cond->waiters);
    return 0;
}

static inline int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    EnterCriticalSection(&cond->waiters);
    cond->waiters_count++;
    LeaveCriticalSection(&cond->waiters);

    LeaveCriticalSection(&mutex->cs);
    WaitForMultipleObjects(2, cond->events, FALSE, INFINITE);
    EnterCriticalSection(&mutex->cs);
    return 0;
}

/* timedwait: basit implementasyon */
static inline int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                                         const struct timespec *abstime)
{
    (void)abstime;

    LeaveCriticalSection(&mutex->cs);

    /* Basit: kısa süre bekle, timeout olursa ETIMEDOUT döndür */
    DWORD wait_ms = 1;
    DWORD res = WaitForMultipleObjects(2, cond->events, FALSE, wait_ms);

    EnterCriticalSection(&mutex->cs);

    if (res == WAIT_TIMEOUT) {
        errno = ETIMEDOUT;
        return -1;
    }
    return 0;
}

static inline int pthread_cond_signal(pthread_cond_t *cond)
{
    EnterCriticalSection(&cond->waiters);
    if (cond->waiters_count > 0) {
        SetEvent(cond->events[0]);
        cond->waiters_count--;
    }
    LeaveCriticalSection(&cond->waiters);
    return 0;
}

static inline int pthread_cond_broadcast(pthread_cond_t *cond)
{
    EnterCriticalSection(&cond->waiters);
    if (cond->waiters_count > 0) {
        SetEvent(cond->events[1]);
        cond->waiters_count = 0;
    }
    LeaveCriticalSection(&cond->waiters);
    return 0;
}

static inline int pthread_kill(pthread_t thread, int sig)
{
    (void)thread;
    (void)sig;
    return 0;
}

static inline int pthread_cancel(pthread_t thread)
{
    TerminateThread((HANDLE)thread, 0);
    return 0;
}

static inline void pthread_exit(void *retval)
{
    (void)retval;
    ExitThread(0);
}

static inline int pthread_testcancel(void)
{
    return 0;
}

static inline int pthread_attr_init(pthread_attr_t *attr)
{
    (void)attr;
    return 0;
}

static inline int pthread_attr_destroy(pthread_attr_t *attr)
{
    (void)attr;
    return 0;
}

static inline int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    (void)attr;
    (void)stacksize;
    return 0;
}

static inline int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    if (once_control->state == 0) {
        once_control->state = 1;
        init_routine();
    }
    return 0;
}

static inline int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    (void)destructor;
    *key = TlsAlloc();
    return (*key == TLS_OUT_OF_INDEXES) ? -1 : 0;
}

static inline int pthread_key_delete(pthread_key_t key)
{
    TlsFree(key);
    return 0;
}

static inline int pthread_setspecific(pthread_key_t key, const void *value)
{
    return TlsSetValue(key, (LPVOID)value) ? 0 : -1;
}

static inline void *pthread_getspecific(pthread_key_t key)
{
    return TlsGetValue(key);
}

static inline int pthread_setname_np(pthread_t thread, const char *name)
{
    (void)thread;
    (void)name;
    return 0;
}

/* --------------- Signal stub'ları --------------- */
/* Not: sigset_t signal.h'da tanımlı olmalı (port.c genelde include eder) */

static inline int sigemptyset(sigset_t *set)
{
    if (set) memset(set, 0, sizeof(sigset_t));
    return 0;
}

static inline int sigfillset(sigset_t *set)
{
    if (set) memset(set, 0xFF, sizeof(sigset_t));
    return 0;
}

static inline int sigaddset(sigset_t *set, int signum)
{
    (void)set; (void)signum;
    return 0;
}

static inline int sigdelset(sigset_t *set, int signum)
{
    (void)set; (void)signum;
    return 0;
}

static inline int sigwait(const sigset_t *set, int *sig)
{
    (void)set;
    (void)sig;
    Sleep(100);
    return 0;
}

static inline int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset)
{
    (void)how; (void)set; (void)oldset;
    return 0;
}

struct sigaction {
    void (*sa_handler)(int);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};

static inline int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
    (void)sig; (void)act; (void)oact;
    return 0;
}

/* --------------- POSIX time stub'ları --------------- */

struct tms {
    clock_t tms_utime;
    clock_t tms_stime;
    clock_t tms_cutime;
    clock_t tms_cstime;
};

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

static inline int clock_gettime(int clock_id, struct timespec *tp)
{
    (void)clock_id;
    FILETIME ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    uli.QuadPart -= 116444736000000000ULL; /* Unix epoch */
    tp->tv_sec = (long)(uli.QuadPart / 10000000ULL);
    tp->tv_nsec = (long)((uli.QuadPart % 10000000ULL) * 100);
    return 0;
}

static inline int usleep(unsigned int usec)
{
    Sleep(usec / 1000);
    return 0;
}

static inline clock_t times(struct tms *buf)
{
    (void)buf;
    return clock();
}

#endif /* HAVE_PTHREAD_H */
#endif /* _WIN32 */

#endif /* PTHREAD_STUB_H */
