#include "LatestFixedQueue.h"
#include <iostream>
#include <vector>

void tc_1()
{
  LatestFixedQueue<std::vector<float>> queue(2);
  std::vector<float> a;
  a.emplace_back(1.0);
  std::vector<float> b;
  b.emplace_back(2.0);
  b.emplace_back(3.0);
  queue.Push(a);

  std::vector<std::vector<float>> data_arr;
  queue.GetItems(
      data_arr, [](std::vector<float> data) {
        return true;
      },
      1);
  for (auto data : data_arr) {
    for (auto item : data) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }

  queue.Push(b);
  queue.GetItems(data_arr, [](std::vector<float> data) {
    return true;
  });
  for (auto data : data_arr) {
    for (auto item : data) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }
}

void tc_2()
{
  LatestFixedQueue<std::vector<float>> queue(2);
  std::vector<float> a;
  a.emplace_back(1.0);
  queue.Push(a);

  std::vector<float> b;
  b.emplace_back(2.0);
  b.emplace_back(3.0);
  queue.Push(b);

  std::vector<float> c;
  c.emplace_back(4.0);
  c.emplace_back(5.0);
  queue.Push(c);

  std::vector<float> d;
  while (queue.Pop(d)) {
    for (auto item : d) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }
}

int main()
{
  // tc_1();
  tc_2();
  return 0;
}
