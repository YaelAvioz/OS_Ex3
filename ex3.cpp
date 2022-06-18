#include "ex3.h"

// Globals
std::vector<Producer *> producers;
BoundedQueue *screen_queue;
UnboundedQueue *news_queue = new UnboundedQueue();
UnboundedQueue *sports_queue = new UnboundedQueue();
UnboundedQueue *weather_queue = new UnboundedQueue();

void UnboundedQueue::enqueue(std::string s) {
  m_mtx.lock();
  m_queue.push(s);
  m_mtx.unlock();
  sem_post(&m_full);
}

std::string UnboundedQueue::dequque() {
  sem_wait(&m_full);
  m_mtx.lock();
  auto s = m_queue.front();
  m_queue.pop();
  m_mtx.unlock();
  return s;
}

void BoundedQueue::enqueue(std::string news) {
  sem_wait(&m_empty);
  UnboundedQueue::enqueue(news);
}

std::string BoundedQueue::dequque() {
  auto s = UnboundedQueue::dequque();
  sem_post(&m_empty);
  return s;
}

std::string Producer::dequque() { return m_queue->dequque(); }

std::string Producer::get_report(std::string type, int count) {
  return "Producer " + std::to_string(m_index) + " " + type + " " +
         std::to_string(count);
}

void Producer::produce() {
  int news_count, sports_count, weather_count = 0;
  for (int i; i < m_products; ++i) {
    auto type = rand() % 3;
    std::string report;
    switch (type) {
    case 0:
      report = get_report(NEWS, news_count);
      news_count++;
      break;
    case 1:
      report = get_report(SPORTS, sports_count);
      sports_count++;
      break;
    case 2:
      report = get_report(WEATHER, weather_count);
      weather_count++;
      break;
    }
    m_queue->enqueue(report);
  }

  m_queue->enqueue("DONE");
}

int Producer::index() { return m_index; }

void dispatcher() {
  while (!producers.empty()) {
    for (auto producer : producers) {
      auto report = producer->dequque();
      if (report == "DONE") {
        producers.erase(producers.begin() + producer->index());
      } else {
        if (report.find(NEWS) != std::string::npos) {
          news_queue->enqueue(report);
        } else if (report.find(SPORTS) != std::string::npos) {
          sports_queue->enqueue(report);
        } else if (report.find(WEATHER) != std::string::npos) {
          weather_queue->enqueue(report);
        }
      }
    }
  }
  news_queue->enqueue("DONE");
  sports_queue->enqueue("DONE");
  weather_queue->enqueue("DONE");
}

void co_editor(UnboundedQueue *dispatcher_queue) {
  while (true) {
    sleep(0.1);
    auto report = dispatcher_queue->dequque();
    screen_queue->enqueue(report);
    if (report == "DONE") {
      break;
    }
  }
}

void screen_manager() {
  int done_co_editors = 0;
  while (done_co_editors < 3) {
    auto report = screen_queue->dequque();
    if (report == "DONE") {
      done_co_editors++;
    } else {
      std::cout << report << std::endl;
    }
  }
  std::cout << "DONE" << std::endl;
}

void initialize(std::ifstream &config) {
  std::string line;
  int products_count;
  int producer_size;
  int screen_size;
  int index = 0;

  while (getline(config, line)) {
    screen_size = stoi(line);
    if (getline(config, line)) {
      products_count = stoi(line);
      getline(config, line);
      producer_size = stoi(line);
      getline(config, line);
      producers.push_back(new Producer(index, products_count, producer_size));
      index++;
    }
  }
  screen_queue = new BoundedQueue(screen_size);

  config.close();
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

  initialize(config);

  std::vector<std::thread> threads;
  for (auto producer : producers) {
    threads.push_back(std::thread(&Producer::produce, producer));
  }

  threads.push_back(std::thread(dispatcher));
  threads.push_back(std::thread(co_editor, news_queue));
  threads.push_back(std::thread(co_editor, sports_queue));
  threads.push_back(std::thread(co_editor, weather_queue));
  threads.push_back(std::thread(screen_manager));

  for (auto &thread : threads) {
    thread.join();
  }

  // TODO: cleanups
}