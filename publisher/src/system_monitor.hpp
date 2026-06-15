#pragma once
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sys/sysinfo.h>
#include "metrics.hpp"

namespace SystemMonitor{

inline float get_temperature()
{
   std::ifstream f("/sys/class/thermal/thermal_zone0/temp");
   if (!f.is_open()){
      std::cerr << "[ERROR] No se puede leer la temperatura\n";
      return 0.0f;
   }
   int raw = 0;
   f >> raw;
   return raw / 1000.0f;
}

inline float get_memory_used_percent()
{
   struct sysinfo info;
   if (sysinfo(&info) != 0){
      std::cerr << "[ERROR] No se puede leer la memoria RAM usada\n";
      return 0.0f;
   }
   float total = static_cast<float>(info.totalram);
   float free  = static_cast<float>(info.freeram);

   return 100.0f * (total - free) / total;
}

inline uint32_t get_running_processes()
{
   struct sysinfo info;
   if (sysinfo(&info) != 0){
      std::cerr << "[ERROR] No se puede leer el uptime del equipo\n";
      return 0;
   }
   return info.procs;
}

inline uint64_t get_uptime_seconds()
{
   struct sysinfo info;
   if (sysinfo(&info) != 0){
      std::cerr << "[ERROR] No se puede leer el uptime del equipo\n";
      return 0;
   }
   return info.uptime;
}

inline Metrics read_all_metrics()
{
   Metrics m {};
   m.cpu_temp             = get_temperature();
   m.memory_usage_percent = get_memory_used_percent();
   m.running_processes    = get_running_processes();
   m.uptime_sec           = get_uptime_seconds();
   return m;
}


}