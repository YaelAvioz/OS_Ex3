#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <semaphore.h>
#include <string>
#include <thread>

#define NEWS "NEWS"
#define SPORTS "SPORTS"
#define WEATHER "WEATHER"

class UnboundedQueue {
public:
  // constractor
  UnboundedQueue() { sem_init(&m_full, 0, 0); };

  // insert string to the buffer
  void enqueue(std::string news);

  // remove string from the buffer
  std::string dequque();

  std::string front();

  // TODO: destructor
  virtual ~UnboundedQueue() {
    sem_destroy(&m_full);
    delete (this);
  };

private:
  sem_t m_full;
  std::mutex m_mtx;
  std::queue<std::string> m_queue;
};

class BoundedQueue : UnboundedQueue {
public:
  // constractor
  BoundedQueue(int size) : UnboundedQueue() { sem_init(&m_empty, 0, size); };

  // insert string to the buffer
  void enqueue(std::string news);

  // remove string from the buffer
  std::string dequque();

  std::string front();

  // TODO: destructor
  ~BoundedQueue() { sem_destroy(&m_empty); };

private:
  sem_t m_empty;
};

class Producer {
public:
  Producer(int products, BoundedQueue *queue)
      : m_products(products), m_queue(queue){};

  void produce();

  std::string dequque();

  std::string front();

private:
  int m_products;
  BoundedQueue *m_queue;
};
