#pragma once
#include <cstdint>
#include <cassert>
#include <cstring>
#include <iostream>
#include <mosquitto.h>

class MQTTClient
{
   static constexpr uint8_t MAX_HOST_LEN = 64;
   
protected:
   mosquitto* mosq {nullptr};
   char m_host [MAX_HOST_LEN] {};
   uint16_t m_port {0};

public:
   MQTTClient(const char* client_id, const char* host, int port);
   virtual ~MQTTClient();
   void connect();
   void disconnect();
};