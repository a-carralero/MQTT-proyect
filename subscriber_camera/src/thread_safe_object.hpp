#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>

// Sólo nos interesa el último valor disponible
template <typename T>
class ThreadSafeObject
{
   T data {};
   std::mutex mut {};
   std::condition_variable cv {};
   bool ready {false};
   bool done {false};

public:
   void set(T& obj){
      {
         std::lock_guard<std::mutex> lock(mut);
         data = std::move(obj);
         ready = true;
      }
      cv.notify_one();
   }

   void wait_and_get(T& obj){
      std::unique_lock<std::mutex> lk(mut);
      while(!ready){
         cv.wait(lk);
      }
      obj = std::move(data);
      ready = false;
   }

   void stop(){
      {
         std::lock_guard<std::mutex> lk(mut);
         done = true;
      }
      cv.notify_all();

   }
};