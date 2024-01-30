#include "LatestFixedQueue.h"
#include <iostream>
#include <vector>

// void tc_1()
// {
//   LatestFixedQueue<std::vector<float>> queue(2);
//   std::shared_ptr<std::vector<float>> a = std::make_shared<std::vector<float>>();
//   a->emplace_back(1.0);
//   std::shared_ptr<std::vector<float>> b = std::make_shared<std::vector<float>>();
//   b->emplace_back(2.0);
//   b->emplace_back(3.0);
//   queue.Push(a);

//   std::vector<std::vector<float>> data_arr;
//   queue.GetItems(
//       data_arr, [](const std::shared_ptr<std::vector<float>>& data_ptr) {
//         return true;
//       },
//       1);
//   for (auto& data : data_arr) {
//     for (auto& item : data) {
//       std::cout << item << " ";
//     }
//     std::cout << std::endl;
//   }

//   queue.Push(b);
//   queue.GetItems(data_arr, [](const std::shared_ptr<std::vector<float>>& data_ptr) {
//     return true;
//   });
//   for (auto& data : data_arr) {
//     for (auto& item : data) {
//       std::cout << item << " ";
//     }
//     std::cout << std::endl;
//   }
// }

void tc_2()
{
  LatestFixedQueue<std::vector<float>> queue(2);
  std::shared_ptr<std::vector<float>> a = std::make_shared<std::vector<float>>();
  a->emplace_back(1.0);
  queue.Push(a);

  std::shared_ptr<std::vector<float>> b = std::make_shared<std::vector<float>>();
  b->emplace_back(2.0);
  b->emplace_back(3.0);
  queue.Push(b);

  std::shared_ptr<std::vector<float>> c = std::make_shared<std::vector<float>>();
  c->emplace_back(4.0);
  c->emplace_back(5.0);
  queue.Push(c);

  std::shared_ptr<std::vector<float>> d;
  while (queue.Pop(d)) {
    for (const auto& item : *(d.get())) {
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
