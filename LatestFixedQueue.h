#include <vector>
#include <mutex>

template <class DataType>
class LatestFixedQueue {
public:
  LatestFixedQueue(int cap)
    : _size(0), _cap(cap), _front(0), _rear(-1), _ring(cap) {}

  ~LatestFixedQueue() {}

  bool IsFull()
  {
    return _size >= _cap;
  }

  bool IsEmpty()
  {
    return _size <= 0;
  }

  void Push(DataType item)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    if (IsFull()) {
      _front = (_front + 1) % _cap;
      _rear = (_rear + 1) % _cap;
      _ring[_rear] = item;
    } else {
      _size++;
      _rear = (_rear + 1) % _cap;
      _ring[_rear] = item;
    }
  }

  bool Pop(DataType& data)
  {
    if (IsEmpty()) {
      return false;
    } else {
      std::lock_guard<std::mutex> lock(_mutex);
      _size--;
      data = _ring[_front];
      _front = (_front + 1) % _cap;
    }
  }

  int Size()
  {
    return _size;
  }

  void GetItems(std::vector<DataType>& data_arr, int maxCount = 0)
  {
    data_arr.clear();
    if (IsEmpty()) {
      return;
    } else {
      std::lock_guard<std::mutex> lock(_mutex);
      int count = 0;
      if (maxCount <= 0 || maxCount > Size()) {
        maxCount = Size();
      }
      int i = _front;
      while (count < maxCount) {
        data_arr.emplace_back(_ring[i]);
        i++;
        i = i % _cap;
        count++;
      }
    }
  }

private:
  int _size;
  int _cap;
  int _front;
  int _rear;
  std::vector<DataType> _ring;
  std::mutex _mutex;
};
