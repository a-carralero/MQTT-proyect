#pragma once
#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>

class Timer
{
   using clk = std::chrono::steady_clock;
   clk::time_point m_start;

public:

   Timer() { start(); }

   void start() {m_start = clk::now(); }

   clk::duration ellapsed() { return m_start - clk::now(); }

   int64_t ellapsed_us() {
      using namespace std::chrono;
      microseconds us = duration_cast<microseconds>(clk::now() - m_start);
      return us.count();
   }

   int64_t waitUntil_us (int64_t us){
      int64_t towait = us - ellapsed_us();
      if (towait < 0) return 0;
      std::this_thread::sleep_for(std::chrono::microseconds(towait));
      return towait;
   }
};