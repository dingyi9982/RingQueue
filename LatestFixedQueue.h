#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

template <typename DataType>
class LatestFixedQueue {
public:
  explicit LatestFixedQueue(int cap)
    : cap_(cap), ring_(cap), is_stopped_(false)
  {
    Clear();
  }

  ~LatestFixedQueue() {}

  bool IsFull() const
  {
    return size_ >= cap_;
  }

  bool IsEmpty() const
  {
    return size_ <= 0;
  }

  void Push(const std::shared_ptr<DataType>& data_ptr)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_stopped_) {
      printf("LatestFixedQueue is stopped, push nothing\n");
      return;
    }
    if (IsFull()) {
      printf("queue is full, remove the oldest data\n");
      ring_[front_] = nullptr;
      front_ = (front_ + 1) % cap_;
    } else {
      size_++;
    }
    rear_ = (rear_ + 1) % cap_;

    ring_[rear_] = data_ptr;
    cv_.notify_one();
  }

  bool Pop(std::shared_ptr<DataType>& data_ptr)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return is_stopped_ || !IsEmpty(); });

    if (is_stopped_) {
      printf("LatestFixedQueue is stopped, pop nothing\n");
      return false;
    }

    size_--;

    data_ptr = ring_[front_];
    ring_[front_] = nullptr;
    front_ = (front_ + 1) % cap_;
    return true;
  }

  int Size() const
  {
    return size_;
  }

  template <typename FilterFunc>
  void GetItems(
      std::vector<DataType>& data_arr,
      FilterFunc filter =
          [](const std::shared_ptr<DataType>& data_ptr) { return true; },
      int maxCount = 0)
  {
    data_arr.clear();
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsEmpty()) {
      return;
    } else {
      int count = 0;
      if (maxCount <= 0 || maxCount > Size()) {
        maxCount = Size();
      }
      int i = front_;
      int j = 0;
      while (count < maxCount && j < Size()) {
        if (filter(ring_[i])) {
          data_arr.emplace_back(*ring_[i]);
          count++;
        }
        i++;
        i = i % cap_;
        j++;
      }
    }
  }

  void Start()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    is_stopped_ = false;

    Clear();
  }

  void Stop(bool after_queue_empty = false)
  {
    if (!after_queue_empty) {
      std::lock_guard<std::mutex> lock(mutex_);
      is_stopped_ = true;
      cv_.notify_all();
      return;
    }

    while (true) {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (IsEmpty()) {
          is_stopped_ = true;
          cv_.notify_all();
          break;
        }
        cv_.notify_all();
        printf("Waiting for queue empty, queue _size=%d\n", Size());
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }

  void SetCapacity(int cap)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    cap_ = cap;
    ring_.resize(cap);
  }

private:
  void Clear()
  {
    size_ = 0;
    front_ = 0;
    rear_ = -1;

    for (auto& data_ptr : ring_) {
      data_ptr = nullptr;
    }
  }

private:
  std::atomic<int> size_;
  int cap_;
  int front_;
  int rear_;
  std::vector<std::shared_ptr<DataType>> ring_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool is_stopped_;
};
