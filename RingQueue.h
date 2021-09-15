#ifndef __RingQueue_H__
#define __RingQueue_H__

#include <vector>

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#endif

template <class DataType>
class RingQueue
{
public:
    RingQueue(int cap);
    ~RingQueue();

    void Reset();

#ifndef __APPLE__
    bool IsEmpty();
    bool IsFull();
#endif

    bool Push(const DataType &data, bool forever = false);
#ifdef __APPLE__
    bool Pop(DataType &data, bool forever = false);
#else
    bool Pop(DataType &data, long msecs = 0);
    void PopAll(std::vector<DataType> &data_arr);
#endif

private:
    int _cap;
    std::vector<DataType> ring;

#ifdef __APPLE__
    dispatch_semaphore_t blank_sem;
    dispatch_semaphore_t data_sem;
#else
    sem_t blank_sem;
    sem_t data_sem;
#endif

    int c_step;
    int p_step;
};

template<class DataType>
void RingQueue<DataType>::Reset()
{
    c_step = p_step = 0;
#ifdef __APPLE__
    blank_sem = dispatch_semaphore_create(_cap);
    data_sem = dispatch_semaphore_create(0);
#else
    sem_init(&blank_sem, 0, _cap);
    sem_init(&data_sem, 0, 0);
#endif
}

template<class DataType>
RingQueue<DataType>::RingQueue(int cap):_cap(cap), ring(cap)
{
    Reset();
}

template<class DataType>
RingQueue<DataType>::~RingQueue()
{
#ifndef __APPLE__
    sem_destroy(&blank_sem);
    sem_destroy(&data_sem);
#endif
}

#ifndef __APPLE__
template<class DataType>
bool RingQueue<DataType>::IsEmpty()
{
    int data_sem_value = 0;
    sem_getvalue(&data_sem, &data_sem_value);
    return data_sem_value == 0;
}

template<class DataType>
bool RingQueue<DataType>::IsFull()
{
    int blank_sem_value = 0;
    sem_getvalue(&blank_sem, &blank_sem_value);
    return blank_sem_value == 0;
}
#endif

template<class DataType>
bool RingQueue<DataType>::Push(const DataType &data, bool forever/* = false*/)
{
#ifdef __APPLE__
    if (!forever) {
        dispatch_semaphore_wait(blank_sem, DISPATCH_TIME_NOW);
    } else {
        dispatch_semaphore_wait(blank_sem, DISPATCH_TIME_FOREVER);
    }
#else
    if ((!forever && !sem_trywait(&blank_sem)) ||
        (forever && !sem_wait(&blank_sem))) {
#endif

    ring[p_step] = data;

#ifdef __APPLE__
    dispatch_semaphore_signal(data_sem);
#else
    sem_post(&data_sem);
#endif

    p_step++;
    p_step %= _cap;
    return true;

#ifndef __APPLE__
    } else {
        return false;
    }
#endif
}

#ifdef __APPLE__
template<class DataType>
bool RingQueue<DataType>::Pop(DataType &data, bool forever/* = false*/)
{
    if (!forever) {
        dispatch_semaphore_wait(data_sem, DISPATCH_TIME_NOW);
    } else {
        dispatch_semaphore_wait(data_sem, DISPATCH_TIME_FOREVER);
    }
    data = ring[c_step];
    dispatch_semaphore_signal(blank_sem);
    c_step++;
    c_step %= _cap;
    return true;
}
#else
template<class DataType>
bool RingQueue<DataType>::Pop(DataType &data, long msecs)
{
    int eval;
    if (msecs == 0) {
        eval = sem_trywait(&data_sem);
    } else if (msecs < 0) {
        eval = sem_wait(&data_sem);
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        long secs = msecs / 1000;
        msecs = msecs % 1000;
        
        long add = 0;
        msecs = msecs * 1000 * 1000 + ts.tv_nsec;
        add = msecs / (1000 * 1000 * 1000);
        ts.tv_sec += (add + secs);
        ts.tv_nsec = msecs % (1000 * 1000 * 1000);

        while ((eval = sem_timedwait(&data_sem, &ts)) == -1 && errno == EINTR) {
            continue;
        }
    }

    if (0 == eval) {
        data = ring[c_step];
        sem_post(&blank_sem);
        c_step++;
        c_step %= _cap;
    } else if (eval == -1 && errno == ETIMEDOUT) {
        // timeout
    }

    return (0 == eval);
}

template<class DataType>
void RingQueue<DataType>::PopAll(std::vector<DataType> &data_arr)
{
    while(1) {
        if (!sem_trywait(&data_sem)) {
            const DataType &data = ring[c_step];
            data_arr.push_back(data);
            sem_post(&blank_sem);
            c_step++;
            c_step %= _cap;
        } else {
            break;
        }
    }
}
#endif

#endif
