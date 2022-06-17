#include <mutex>
#include <queue>
#include <semaphore.h>
#include <string>

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

  // TODO: destructor
  ~BoundedQueue() { sem_destroy(&m_empty); };

private:
  sem_t m_empty;
};
