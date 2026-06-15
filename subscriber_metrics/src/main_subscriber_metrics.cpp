#include <iostream>
#include "Mosquitto/src/subscriber.hpp"
#include "metrics.hpp"

void callback(const char* topic, uint8_t* payload,
               int payload_len, int qos          )
{
   (void)topic;
   (void)qos;

   if (payload_len != sizeof(Metrics)){
      std::cerr << "[Error] Tamaño del payload inesperado: "
                << payload_len << " (esperado " 
                << sizeof(Metrics) << ")\n";
      return;
   }

   Metrics m {};
   std::memcpy(&m, payload, sizeof(Metrics));
   
   std::cout << "---------------------------------\n"
             << "CPU temp      : " << m.cpu_temp << " ºC\n"
             << "Memory usage  : " << m.memory_usage_percent << " %\n"
             << "Running procs : " << m.running_processes << "\n"
             << "Uptime        : " << m.uptime_sec << " s\n"
             << "---------------------------------\n";
}


int main()
{
   const char BROKER_HOST[] = "localhost";
   const int BROKER_PORT = 1883;
   const char TOPIC_METRICS[] = "cpu/metrics";

   Subscriber subscriber("MetricsSubscriber", BROKER_HOST, BROKER_PORT);
   subscriber.setCallback(callback);
   subscriber.connect();
   subscriber.subscribe(TOPIC_METRICS);
   subscriber.loop_forever();  // queda bloqueado, este proceso sólo escucha métricas
   return 0;
}
