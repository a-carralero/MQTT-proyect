#include <iostream>
#include <mutex>
#include <cstring>
#include <vector>
#include <cstdint>
#include <csignal>

#include "shared_frame.hpp"
#include "frame_header.hpp"
#include "camera_thread.hpp"
#include "Mosquitto/src/subscriber.hpp"

ThreadSafeObject<SharedFrame> shared_buff {};

static void callback(const char* topic, uint8_t* payload,
                     int payload_len, int qos          )
{
   (void)topic;
   (void)qos;

   // validar tamaño mínimo
   if (payload_len < sizeof(FrameHeader)){
      std::cerr << "[ERROR] Payload demasiado pequeño: " << payload_len <<"\n";
      return;
   }

   // Extraer header
   FrameHeader header {};
   std::memcpy(&header, payload, sizeof(FrameHeader));

   const uint8_t* jpeg_start = payload + sizeof(FrameHeader);
   const uint32_t jpeg_len = payload_len - sizeof(FrameHeader);
   
   // Comprobación de tamaño
   if (jpeg_len != header.jpeg_size){
      std::cerr << "[ERROR] jpeg_size del header no coincide con el payload\n";
      return;
   }

   static uint32_t last_frame_id = UINT32_MAX;
   // detectar header_duplicado
   if (last_frame_id == header.frame_id){
      std::cerr << "[WARN]: El publisher envió de nuevo el mismo frame - return...\n";
      return;
   }

   SharedFrame frame {};
   frame.jpeg_data.assign(jpeg_start, jpeg_start + jpeg_len);
   frame.frame_id = header.frame_id;

   // lo añadimos al buffer para que lo procese el thread correspondiente
   shared_buff.set(frame);
}



int main()
{
   const char BROKER_HOST[] = "localhost";
   const int BROKER_PORT = 1883;
   const char TOPIC_CAMERA[] = "camera/frame";

   CameraThread cam_thread (shared_buff);
   cam_thread.start();

   Subscriber subs("CameraSubscriber", BROKER_HOST, BROKER_PORT);
   subs.setCallback(callback);
   subs.connect();
   subs.subscribe(TOPIC_CAMERA);
   subs.loop_daemon();  // El network thread se ejecuta como demonio

   sigset_t set;
   sigemptyset(&set);
   sigaddset(&set, SIGINT);
   sigaddset(&set, SIGTERM);
   pthread_sigmask(SIG_BLOCK,&set,nullptr);

   int sig;
   sigwait(&set, &sig); // Esperamos hasta la señal

   std::cout << "\nSeñal SIGINT/SIGTERM detectada\n";
   shared_buff.stop();
   cam_thread.stop();

   return 0;
}
