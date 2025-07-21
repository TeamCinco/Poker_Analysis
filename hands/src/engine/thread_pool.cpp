#include "thread_pool.hpp"

namespace poker {

ThreadPool::ThreadPool(size_t threads) : stop_(false), active_tasks_(0) {
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this] {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    
                    this->condition_.wait(lock, [this] { 
                        return this->stop_ || !this->tasks_.empty(); 
                    });
                    
                    if (this->stop_ && this->tasks_.empty()) {
                        return;
                    }
                    
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }

                task();
                
                // Signal task completion
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    active_tasks_--;
                    if (active_tasks_ == 0) {
                        finished_.notify_all();
                    }
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    
    condition_.notify_all();
    
    for (std::thread &worker : workers_) {
        worker.join();
    }
}

void ThreadPool::wait_for_all() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    finished_.wait(lock, [this] { 
        return active_tasks_ == 0 && tasks_.empty(); 
    });
}

} // namespace poker
