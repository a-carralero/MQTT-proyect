#pragma once
#include "mqtt_client.hpp"

using MessageCallback = void (*)(const char* topic,
                                 uint8_t* payload,
                                 int         payload_len,
                                 int         qos);

class Subscriber : public MQTTClient
{
   MessageCallback user_cb {};
public:
   Subscriber(const char* client_id, const char* host, int port);
   void subscribe(const char* topic, int qos=0);
   void loop_forever();
   void loop_daemon();
   void setCallback(MessageCallback cb);

private:
   static void on_connect_cb(mosquitto*, void*, int rc);
   static void on_message_cb(mosquitto*, void* userdata,
                             const mosquitto_message* msg);
};