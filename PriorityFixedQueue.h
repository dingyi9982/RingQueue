#include <queue>

template <typename DataType>
class PriorityFixedQueue {
public:
  PriorityFixedQueue(int cap) : _cap(cap) {}
  void Push(DataType value) {
    if (static_cast<int>(_q.size()) < _cap)
      _q.push(value);
    else if (value > _q.top()) {
      _q.pop();
      _q.push(value);
    }
  }

  void PopAll(std::vector<DataType>& data_arr) {
    data_arr.resize(_q.size());
    while (_q.size()) {
      data_arr[_q.size() - 1] = _q.top();
      _q.pop();
    }
  }

private:
  int _cap;
  std::priority_queue<DataType, std::vector<DataType>, std::greater<DataType>> _q;
};
