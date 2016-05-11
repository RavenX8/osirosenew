// Copyright 2016 Chirstopher Torres (Raven), L3nn0x
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _thread_pool_h_
#define _thread_pool_h_
#include <thread>
#include <asio.hpp>
#include <queue>
#include <mutex>
#include <atomic>

namespace Core {
#define MAX_NETWORK_THREADS 512

class NetworkThreadPool {
  typedef std::unique_ptr<asio::io_service::work> asio_worker;

 public:
  static NetworkThreadPool& GetInstance(uint16_t maxthreads = 0) {
    if (instance_ == nullptr) instance_ = new NetworkThreadPool(maxthreads);
    return *instance_;
  }

  static void DeleteInstance() {
    if (instance_ != nullptr) {
      delete instance_;
      instance_ = nullptr;
    }
  }

  asio::io_service* Get_IO_Service() { return &io_service_; }
  uint16_t GetThreadCount() { return threads_running_; }

 private:
  NetworkThreadPool(uint16_t maxthreads) : io_work_(new asio_worker::element_type(io_service_)) {
    uint16_t core_count = std::thread::hardware_concurrency()
               ? std::thread::hardware_concurrency()
               : 1;;

    core_count *= 2;

    if(maxthreads != 0 && core_count > maxthreads)
      core_count = maxthreads;

    if (core_count > MAX_NETWORK_THREADS)
      core_count = MAX_NETWORK_THREADS;
    else if (core_count == 0)
      core_count = 1;

    threads_running_ = core_count;
    for (uint32_t idx = 0; idx < core_count; ++idx) {
      io_thread_[idx] = std::thread([this]() {
        ++threads_running_;
        io_service_.run();
        --threads_running_;
      }); //todo(raven): change this to poll and loop while we are active.
    }
  }

  ~NetworkThreadPool() { Shutdown(); }

  void Shutdown() {
    io_work_.reset();

    for (int idx = 0; idx < threads_running_; ++idx) {
      io_thread_[idx].join();
    }
    
    io_service_.stop();
  }

  std::atomic<int> threads_running_;
  std::thread io_thread_[MAX_NETWORK_THREADS];
  asio::io_service io_service_;
  asio_worker io_work_;
  static NetworkThreadPool* instance_;
};
}

#endif  // __thread_pool_h__
