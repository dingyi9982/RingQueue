#include "PriorityFixedQueue.h"
#include <iostream>

int main()
{
  DescendingFixedQueue<int> descending_queue(3);
  AscendingFixedQueue<int> ascending_queue(3);

  int values[] = {3, 4, 7, 2, 5};
  for(int value: values) {
    descending_queue.Push(value);
    ascending_queue.Push(value);
  }

  std::vector<int> descending_data_arr;
  std::vector<int> ascending_data_arr;
  descending_queue.PopAll(descending_data_arr);
  ascending_queue.PopAll(ascending_data_arr);
  std::cout << "descending: ";
  for(auto data: descending_data_arr) {
    std::cout << data << " ";
  }
  std::cout << std::endl;
  std::cout << "ascending: ";
  for(auto data: ascending_data_arr) {
    std::cout << data << " ";
  }
  std::cout << std::endl;

  return 0;
}
