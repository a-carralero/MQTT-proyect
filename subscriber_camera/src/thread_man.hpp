#pragma once
#include <thread>
#include <atomic>
#include <iostream>

class ThreadMan
{
private:
   std::thread m_thread;
   virtual void run() = 0;

protected:
   std::atomic<bool> m_running {false};

public:
   virtual ~ThreadMan() = default; 

   void start()
   {
      m_running = true;
      if (m_thread.joinable()){
         std::cerr << " The thread is already active\n";
      }
      else {
         m_thread = std::thread(&ThreadMan::run, this);
      }
   }

   void stop()
   {
      m_running = false;
      if (m_thread.joinable())
         m_thread.join();
   }
};
