#pragma once
#include <cstdint>

// Struct compartido entre el publisher y el subscriber_metrics
// __attribute__((packed)) elimina el padding del compilador
// para que el layout de memoria sea idéntico en el publisher y en el subscriber

struct Metrics
{
   float cpu_temp;              // ºC  4 bytes 
   float memory_usage_percent;  // MB  4 bytes
   uint32_t running_processes;  //     4 bytes
   uint64_t uptime_sec;         // s   8 bytes

}__attribute__((packed));    // 20 bytes en total