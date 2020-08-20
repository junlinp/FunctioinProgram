#ifndef FUNCTION_PROGRAM_THREAD_POOL_H_
#define FUNCTION_PROGRAM_THREAD_POOL_H_

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
 public:
  ThreadPool(size_t max_thread_num = 1)
      : max_thread_num_(max_thread_num), b_stop(false) {
    auto thread_base_task = [this]() {
      while (true) {
        std::function<void(void)> task;
        {
          std::unique_lock<std::mutex> ulk(mutex_);
          cv_.wait(ulk, [this]() { return b_stop || !tasks.empty(); });

          if (b_stop && tasks.empty()) {
            return;
          }
          task = std::move(tasks.front());
          tasks.pop();
        }
        task();
      }
    };
    for (size_t i = 0; i < max_thread_num_; i++) {
      works.emplace_back(thread_base_task);
    }
  }
  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> ulk(mutex_);
      b_stop = true;
      cv_.notify_all();
    }
    for (auto& thread : works) {
      thread.join();
    }
  }
  template <class Functor, class... Args,
            class ReturnType =
                std::result_of_t<std::remove_reference_t<Functor>(Args...)>>
  std::future<ReturnType> Enqueue(Functor&& functor, Args&&... args) {
    auto f =
        std::bind(std::forward<Functor>(functor), std::forward<Args>(args)...);
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(f);
    auto future = task->get_future();
    {
      std::unique_lock<std::mutex> ulk(mutex_);
      if (b_stop) {
        throw std::runtime_error("ThreadPool has stopped.");
      }
      tasks.push([task]() { (*task)(); });
    }
    cv_.notify_one();
    return future;
  }

  template <class Functor, class ReturnType = std::result_of_t<
                               std::remove_reference_t<Functor>()>>
  std::future<ReturnType> Enqueue(Functor&& functor) {
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(functor);
    auto future = task->get_future();
    {
      std::unique_lock<std::mutex> ulk(mutex_);
      if (b_stop) {
        throw std::runtime_error("ThreadPool has stopped.");
      }
      tasks.push([task]() { (*task)(); });
    }
    cv_.notify_one();
    return future;
  }

 private:
  size_t max_thread_num_;
  std::queue<std::function<void(void)>> tasks;
  std::vector<std::thread> works;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool b_stop;
};
#endif  // FUNCTION_PROGRAM_THREAD_POOL_H_