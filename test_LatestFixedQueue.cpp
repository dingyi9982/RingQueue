#include "LatestFixedQueue.h"
#include <iostream>
#include <vector>

int main()
{
  LatestFixedQueue<std::vector<float>> queue(2);
  std::vector<float> a;
  a.emplace_back(1.0);
  std::vector<float> b;
  b.emplace_back(2.0);
  b.emplace_back(3.0);
  queue.Push(a);

  std::vector<std::vector<float>> data_arr;
  // queue.GetItems(data_arr, 1);
  queue.GetItems(data_arr, [](std::vector<float> data) -> bool {
                  return true;
                }, 1);
  for (auto data : data_arr) {
    for (auto item : data) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }

  queue.Push(b);
  queue.GetItems(data_arr, [](std::vector<float> data) -> bool {
                  return true;
                });
  for (auto data : data_arr) {
    for (auto item : data) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
