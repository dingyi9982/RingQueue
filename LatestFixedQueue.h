#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

template <typename DataType>
class LatestFixedQueue {
public:
  explicit LatestFixedQueue(int cap)
    : cap_(cap), ring_(cap), is_stopped_(false), is_stopping_(false)
  {
    ClearInternal();
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
    if (is_stopping_.load() || is_stopped_) {
      printf("LatestFixedQueue is stopped, push nothing\n");
      return;
    }
    if (IsFull()) {
      static auto last = std::chrono::steady_clock::now();
      auto elspsed = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - last);
      if (elspsed.count() > 1000) {
        last = std::chrono::steady_clock::now();
        printf("queue is full, remove the oldest data");
      }
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

  void GetItems(
      std::vector<DataType>& data_arr,
      std::function<bool(const std::shared_ptr<DataType>& data_ptr)> filter =
          [](const std::shared_ptr<DataType>& data_ptr) {
            (void)data_ptr;
            return true;
          },
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

  bool Start()
  {
    if (is_stopping_.load()) {
      printf("start failed, because LatestFixedQueue is stopping");
      return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    is_stopped_ = false;

    ClearInternal();
    return true;
  }

  void Stop(bool after_queue_empty = false, int max_wait_ms = 0)
  {
    if (!after_queue_empty) {
      std::lock_guard<std::mutex> lock(mutex_);
      is_stopped_ = true;
      ClearInternal();
      cv_.notify_all();
      return;
    }

    auto start_time = std::chrono::steady_clock::now();
    while (true) {
      is_stopping_.store(true);
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (IsEmpty()) {
          is_stopped_ = true;
          is_stopping_.store(false);
          cv_.notify_all();
          break;
        }
        cv_.notify_all();
        printf("Waiting for queue empty, queue _size=%d\n", Size());
      }
      
      if (max_wait_ms > 0) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (elapsed >= max_wait_ms) {
          printf("Stop timeout after %d ms, force stop\n", max_wait_ms);
          std::lock_guard<std::mutex> lock(mutex_);
          is_stopped_ = true;
          is_stopping_.store(false);
          cv_.notify_all();
          break;
        }
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
  
  void Clear()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    ClearInternal();
  }

private:
  void ClearInternal()
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
  std::atomic<bool> is_stopping_;
};
