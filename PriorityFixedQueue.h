#include <queue>
#include <mutex>

template <typename DataType> class DescendingFixedQueue {
public:
  DescendingFixedQueue(int cap) : _cap(cap) {}
  
void Push(const DataType& value) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (static_cast<int>(_q.size()) < _cap)
      _q.push(value);
    else if (value > _q.top()) {
      _q.pop();
      _q.push(value);
    }
  }

  void PopAll(std::vector<DataType>& data_arr) {
    std::lock_guard<std::mutex> lock(_mutex);
    data_arr.resize(_q.size());
    while (_q.size()) {
      data_arr[_q.size() - 1] = _q.top();
      _q.pop();
    }
  }

private:
  int _cap;
  std::mutex _mutex;
  std::priority_queue<DataType, std::vector<DataType>, std::greater<DataType>> _q;
};

template <typename DataType> class AscendingFixedQueue {
public:
  AscendingFixedQueue(int cap) : _cap(cap) {}

  void Push(const DataType& value) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (static_cast<int>(_q.size()) < _cap)
      _q.push(value);
    else if (value < _q.top()) {
      _q.pop();
      _q.push(value);
    }
  }

  void PopAll(std::vector<DataType>& data_arr) {
    std::lock_guard<std::mutex> lock(_mutex);
    data_arr.resize(_q.size());
    while (_q.size()) {
      data_arr[_q.size() - 1] = _q.top();
      _q.pop();
    }
  }

private:
  int _cap;
  std::mutex _mutex;
  std::priority_queue<DataType, std::vector<DataType>, std::less<DataType>> _q;
};
