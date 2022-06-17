#include <mutex>
#include <queue>
#include <semaphore.h>
#include <string>

class UnboundedQueue {
public:
  // constractor
  UnboundedQueue() { sem_init(&full, 0, 0); };

  // insert string to the buffer
  void enqueue(std::string news);

  // remove string from the buffer
  std::string dequque();

  // TODO: destructor
  virtual ~UnboundedQueue() {
    sem_destroy(&full);
    delete (this);
  };

private:
  sem_t full;
  std::mutex mtx;
  std::queue<std::string> queue;
};

class BoundedQueue : UnboundedQueue {
public:
  // constractor
  BoundedQueue(int size) : UnboundedQueue() { sem_init(&empty, 0, size); };

  // TODO: destructor
  ~BoundedQueue() { sem_destroy(&empty); };

private:
  sem_t empty;
};
