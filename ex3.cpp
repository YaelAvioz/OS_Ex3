#include "ex3.h"

void UnboundedQueue::enqueue(std::string news) {
  m_mtx.lock();
  m_queue.push(news);
  m_mtx.unlock();
  sem_post(&m_full);
}

std::string UnboundedQueue::dequque() {
  sem_wait(&m_full);
  m_mtx.lock();
  auto news = m_queue.front();
  m_queue.pop();
  m_mtx.unlock();
  return news;
}

std::string UnboundedQueue ::front() { return m_queue.front(); }

void BoundedQueue::enqueue(std::string news) {
  sem_wait(&m_empty);
  UnboundedQueue::enqueue(news);
}

std::string BoundedQueue::dequque() {
  auto news = UnboundedQueue::dequque();
  sem_post(&m_empty);
  return news;
}