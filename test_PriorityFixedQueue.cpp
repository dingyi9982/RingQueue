#include "PriorityFixedQueue.h"
#include <iostream>

int main()
{
  PriorityFixedQueue<int> queue(3);
  queue.Push(3);
  queue.Push(4);
  queue.Push(7);
  queue.Push(2);
  queue.Push(5);

  std::vector<int> data_arr;
  queue.PopAll(data_arr);
  for(auto data: data_arr) {
    std::cout << data << " ";
  }
  std::cout << std::endl;

  return 0;
}
