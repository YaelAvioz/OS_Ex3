#include <string>
#include <queue>
#include <semaphore.h>


class UnbondedQueue {
    public:
        //constractor
        UnbondedQueue() {
            sem_init(&this->full, 0, 0);
        };

        //insert string to the buffer
        void enqueue(std::string news);

        //remove string from the buffer
        std::string dequque();
        
        // TODO: destructor
        ~UnboundedQueue(){
        
    }

    private:
        sem_t full;
        mutex_t mutex;
        std::queue<std::string> queue;
};

class BoundedQueue : UnbondedQueue {
    public:
        //constractor
        BoundedQueue(int size) {
            sem_init(&this->full, 0, 0);
            sem_init(&this->empty, 0, size);
        };

        // TODO: destructor
        ~BoundedQueue(){
        
    }


    private:
        sem_t empty;
}

