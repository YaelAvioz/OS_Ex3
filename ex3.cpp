#include "ex3.h"

void UnboundedQueue::enqueue(std::string news) {
    mtx.lock();
    queue.push(news);
    mtx.unlock();
    sem_post(&full);
}

