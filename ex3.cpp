#include "ex3.h"

#define NEWS "NEWS"
#define SPORTS "SPORTS"
#define WEATHER "WEATHER"

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

std::string UnboundedQueue::front() { return m_queue.front(); }

void BoundedQueue::enqueue(std::string news) {
  sem_wait(&m_empty);
  UnboundedQueue::enqueue(news);
}

std::string BoundedQueue::dequque() {
  auto news = UnboundedQueue::dequque();
  sem_post(&m_empty);
  return news;
}

void producer(int index, int total_count) {
  auto producer_queue = producer_queues[index];
  int news_count, sports_count, wheather_count = 0;
  for (int i; i < total_count; ++i) {
    auto type = rand() % 3;
    auto count_str = std::to_string(i);
    std::string report;
    switch (type) {
    case 0:
      report = count_str + " " + NEWS + " " + std::to_string(news_count++);
      break;
    case 1:
      report = count_str + " " + SPORTS + " " + std::to_string(sports_count++);
      break;
    case 2:
      report =
          count_str + " " + WEATHER + " " + std::to_string(wheather_count++);
      break;
    }
    producer_queue->enqueue(report);
  }

  producer_queue->enqueue(std::to_string(-1));
}

void dispatcher() {
  while (!producer_queues.empty()) {
    for (int index = 0; index < producer_queues.size(); ++index) {
      auto producer_queue = producer_queues[index];
      auto report = producer_queue->front();
      if (report == "-1") {
        producer_queues.erase(producer_queues.begin() + index);
      } else {
        auto report = producer_queue->dequque();
        if (report.find(NEWS) != std::string::npos) {
          news_queue->enqueue(report);
        } else if (report.find(SPORTS) != std::string::npos) {
          sports_queue->enqueue(report);
        } else if (report.find(WEATHER) != std::string::npos) {
          weather_queue->enqueue(report);
        }
      }
    }

    news_queue->enqueue(std::to_string(-1));
    sports_queue->enqueue(std::to_string(-1));
    weather_queue->enqueue(std::to_string(-1));
  }
}