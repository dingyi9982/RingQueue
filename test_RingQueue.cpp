#include "test_RingQueue.h"
#include "RingQueue.h"
#include <time.h>
#include <unistd.h>
#include <string>
#include <pthread.h>

#define POP_ALL 0
const int num = 3;

typedef struct {
    std::string url;
    std::string username;
    std::string passwd;
} TurnServer;

void *consume_routine(void* arg)
{
  RingQueue<std::string> *q = (RingQueue<std::string> *)arg;
  while(1)
  {
  #if POP_ALL
    std::vector<std::string> data_arr;
    q->PopAll(data_arr);
    if (data_arr.size() == 0) {
      fprintf(stdout, "\npop failed: no data");
      fflush(stdout);
    } else {
      for (const std::string &data: data_arr) {
        fprintf(stdout, "\npop: %s", data.c_str());
        fflush(stdout);
      }
    }
    usleep(rand() % 400000 + 50000);
  #else
    std::string data;
    if (!q->Pop(data, 50)) {
        fprintf(stdout, "\npop failed: timeout");
        fflush(stdout);
    } else {
        fprintf(stdout, "\npop: %s", data.c_str());
        fflush(stdout);
    }
  #endif
  }
}

void *product_routine(void* arg)
{
  RingQueue<std::string> *q = (RingQueue<std::string> *)arg;
  srand((unsigned)time(NULL));
  while(1)
  {
    const std::string &data = std::to_string(rand() % 100 + 1);
    bool success = q->Push(data);
    
    if (success) {
        fprintf(stdout, "\npush successful: %s", data.c_str());
    } else {
        fprintf(stdout, "\npush failed: %s", data.c_str());
    }
    fflush(stdout);

    usleep(rand() % 100000 + 1);
  }
}

void test_RingQueue()
{
  RingQueue<std::string> *q = new RingQueue<std::string>(num);

  pthread_t c, p;

  pthread_create(&c, NULL, consume_routine, (void*)q);
  pthread_create(&p, NULL, product_routine, (void*)q);

  pthread_join(c, NULL);
  pthread_join(p, NULL);

  delete(q);
  q = NULL;
}

int main()
{
    test_RingQueue();
    return 0;
}
