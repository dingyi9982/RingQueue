#ifndef __RingQueue_H__
#define __RingQueue_H__

#include <vector>
#include <semaphore.h>

template <class DataType>
class RingQueue
{
public:
    RingQueue(int cap);
    ~RingQueue();

    void Reset();

    bool IsEmpty();
    bool IsFull();
    bool PushData(const DataType &data, bool forever = false);
    bool PopOneData(DataType &data, bool forever = false);
    void PopData(std::vector<DataType> &data_arr, bool forever = false);

private:
    int _cap;
    std::vector<DataType> ring;
    sem_t blank_sem;
    sem_t data_sem;
    int c_step;
    int p_step;
};

template<class DataType>
void RingQueue<DataType>::Reset()
{
    c_step = p_step = 0;
    sem_init(&blank_sem, 0, _cap);
    sem_init(&data_sem, 0, 0);
}

template<class DataType>
RingQueue<DataType>::RingQueue(int cap):_cap(cap), ring(cap)
{
    Reset();
}

template<class DataType>
RingQueue<DataType>::~RingQueue()
{
    sem_destroy(&blank_sem);
    sem_destroy(&data_sem);
}

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

template<class DataType>
bool RingQueue<DataType>::PushData(const DataType &data, bool forever/* = false*/)
{
    if ((!forever && !sem_trywait(&blank_sem)) ||
        (forever && !sem_wait(&blank_sem))) {
        ring[p_step] = data;
        sem_post(&data_sem);
        p_step++;
        p_step %= _cap;
        return true;
    } else {
        return false;
    }
}

template<class DataType>
bool RingQueue<DataType>::PopOneData(DataType &data, bool forever/* = false*/)
{
    bool ret = false;
    if ((!forever && !sem_trywait(&data_sem)) ||
        (forever && !sem_wait(&data_sem))) {
        data = ring[c_step];
        sem_post(&blank_sem);
        c_step++;
        c_step %= _cap;
        ret = true;
    }

    return ret;
}

template<class DataType>
void RingQueue<DataType>::PopData(std::vector<DataType> &data_arr, bool forever/* = false*/)
{
    while(1) {
        if ((!forever && !sem_trywait(&data_sem)) ||
            (forever && !sem_wait(&data_sem))) {
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
