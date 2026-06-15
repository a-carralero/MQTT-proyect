#include "mqtt_client.hpp"

MQTTClient::MQTTClient(const char* client_id, const char* host, int port)
   : m_port(port)
{
   assert(strlen(host) <= MAX_HOST_LEN);
   std::memcpy(m_host, host, strlen(host));
   
   mosquitto_lib_init();
   mosq = mosquitto_new(client_id, true, this);
   if (!mosq){
      std::cerr << "[ERROR] No se pudo crear el cliente mosquitto\n";
      std::terminate();
   }   
}

MQTTClient::~MQTTClient(){
   if (mosq){
      mosquitto_destroy(mosq);
      mosq = nullptr;
   }
   mosquitto_lib_cleanup();
}

void MQTTClient::connect(){
   int rc = mosquitto_connect(mosq, m_host, m_port, 60);
   if (rc != MOSQ_ERR_SUCCESS) {
      std::cerr << "[ERROR] No se pudo conectar al broker: " 
                  << mosquitto_strerror(rc) << "\n";
      mosquitto_destroy(mosq);
      mosquitto_lib_cleanup();
      std::terminate();
   }
   std::cout << "[OK] Conectado al broker " << m_host << ":" << m_port << "\n";
}

void MQTTClient::disconnect(){
   mosquitto_disconnect(mosq);
} 