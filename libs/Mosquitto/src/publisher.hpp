#pragma once
#include "mqtt_client.hpp"

class Publisher : public MQTTClient
{

public:
   Publisher(const char* client_id, const char* host, int port)
      : MQTTClient(client_id, host, port)
   {}
   
   template <typename T>
   void publish(const char* topic, const T& data, int qos=0){
      int rc = mosquitto_publish(mosq, nullptr, topic, sizeof(data), &data, qos, false);
      error_handle(topic, sizeof(data), rc);
   }

   void publish_raw(const char* topic,
                    const uint8_t* data, int len, int qos=0)
   {
      int rc = mosquitto_publish(mosq, nullptr, topic, len, data, qos, false);
      error_handle(topic, len, rc);
   }

   void error_handle(const char* topic, int len, int rc)
   {
      if (rc != MOSQ_ERR_SUCCESS){
         std::cerr << "[ERROR] Fallo al publicar: " << mosquitto_strerror(rc) << "\n";
      }
      else {
         std::cout << "[PUB] " << topic << " " << len << " bytes\n";
      }
   }
};