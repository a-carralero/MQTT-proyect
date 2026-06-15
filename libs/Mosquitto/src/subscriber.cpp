#include "subscriber.hpp"

Subscriber::Subscriber(const char* client_id, const char* host, int port)
   : MQTTClient(client_id, host, port)
{
   mosquitto_connect_callback_set(mosq, on_connect_cb);
   mosquitto_message_callback_set(mosq, on_message_cb);
}

void Subscriber::subscribe(const char* topic, int qos){
   int rc = mosquitto_subscribe(mosq, nullptr, topic, qos);
   if (rc != MOSQ_ERR_SUCCESS){
      std::cerr << "[ERROR] Fallo al suscribirse a: " << topic << "\n";
   }
   else {
      std::cout << "[SUB] Suscrito a " << topic << "\n";
   }
}

void Subscriber::loop_forever(){
   mosquitto_loop_forever(mosq,-1, 1);
}

void Subscriber::loop_daemon(){
   mosquitto_loop_start(mosq);
}

void Subscriber::setCallback(MessageCallback cb){
   user_cb = cb;
}

void Subscriber::on_connect_cb(mosquitto*, void*, int rc){
   if (rc == 0){
      std::cout << "[OK] Subscriber conectado al brocker\n";
   } else {
      std::cerr << "[ERROR] Conexion rechazada, codigo: " << rc << "\n";
   }
}

void Subscriber::on_message_cb(mosquitto*, void* userdata,
                             const mosquitto_message* msg)
{
   auto* self = reinterpret_cast<Subscriber*>(userdata);
   if (!msg || !msg->payload || !self->user_cb) {
      std::cerr << "[ERROR] on_message_cb\n";
      return;
   }
   self->user_cb(msg->topic,
                  reinterpret_cast<uint8_t*>(msg->payload),
                  msg->payloadlen,
                  msg->qos);
}