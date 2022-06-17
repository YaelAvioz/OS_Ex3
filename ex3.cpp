#include "ex3.h"

// Globals
std::vector<BoundedQueue *> producer_queues;
BoundedQueue *news_queue;
BoundedQueue *sports_queue;
BoundedQueue *weather_queue;
UnboundedQueue *screen_queue;

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

void producer(int i, int total_count) {
  auto producer_queue = producer_queues[i];
  int news_count, sports_count, wheather_count = 0;
  for (int i; i < total_count; ++i) {
    auto type = rand() % 3;
    std::string report;
    switch (type) {
    case 0:
      report = std::to_string(i) + " NEWS " + std::to_string(news_count++);
      break;
    case 1:
      report = std::to_string(i) + " SPORTS " + std::to_string(sports_count++);
      break;
    case 2:
      report =
          std::to_string(i) + " WEATHER " + std::to_string(wheather_count++);
      break;
    }
    producer_queue->enqueue(report);
  }

  producer_queues[i]->enqueue(std::to_string(-1));
}
