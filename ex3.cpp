#include "ex3.h"

// Globals
std::vector<Producer *> producers;
UnboundedQueue *news_queue;
UnboundedQueue *sports_queue;
UnboundedQueue *weather_queue;
BoundedQueue *screen_queue;

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

std::string Producer::dequque() { return m_queue->dequque(); }

std::string Producer::front() { return m_queue->front(); }

void Producer::produce() {
  int news_count, sports_count, wheather_count = 0;
  for (int i; i < m_products; ++i) {
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
    m_queue->enqueue(report);
  }

  m_queue->enqueue(std::to_string(-1));
}

void dispatcher() {
  while (!producers.empty()) {
    for (int index = 0; index < producers.size(); ++index) {
      auto producer = producers[index];
      auto report = producer->front();
      if (report == "-1") {
        producers.erase(producers.begin() + index);
      } else {
        auto report = producer->dequque();
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

void co_editor(BoundedQueue *dispatcher_queue) {
  while (true) {
    auto report = dispatcher_queue->dequque();
    screen_queue->enqueue(report);
    if (report == "-1") {
      break;
    }
  }
}

void screen_manager() {
  int done_repoters_count = 0;
  while (done_repoters_count < 3) {
    auto report = screen_queue->dequque();
    if (report == "-1") {
      done_repoters_count++;
    } else {
      std::cout << report << std::endl;
    }
  }
  std::cout << "DONE" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    exit(1);
  }

  std::ifstream config;
  config.open(argv[1]);
  if (!config.is_open()) {
    return -1;
  }

  std::string line;
  int products_count;

  while (getline(config, line)) {
    getline(config, line);
    if (getline(config, line)) {
      products_count = stoi(line);
    } else {
      screen_queue = new BoundedQueue(stoi(line));
      break;
    }
    if (getline(config, line)) {
      producers.push_back(
          new Producer(products_count, new BoundedQueue(stoi(line))));
    }
  }

  config.close();

  std::vector<std::thread> threads;
  for (auto producer : producers) {
    threads.push_back(std::thread(Producer::produce));
  }
}