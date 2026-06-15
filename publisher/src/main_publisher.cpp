#include <iostream>
#include <atomic>
#include <csignal>
#include <vector>
#include <chrono>
#include <cstring>
#include <opencv2/opencv.hpp>

#include "system_monitor.hpp"
#include "frame_header.hpp"
#include "timer.hpp"
#include "Mosquitto/src/publisher.hpp"


static std::atomic<bool> running {true};
void signal_handler(int){
   running = false;
}
static void publish_camera_frame(cv::VideoCapture& cam,
                     Publisher& pub, const char* topic );

int main()
{
   std::signal(SIGINT, signal_handler);
   std::signal(SIGTERM, signal_handler);


   const char BROKER_HOST[]    = "localhost";
   const uint16_t  BROKER_PORT = 1883;
   const char TOPIC_METRICS[]  = "cpu/metrics";
   const char TOPIC_CAMERA[]   = "camera/frame";
   const int CAMERA_DEVICE     = 0; //  /dev/video0
   const double TARGET_FPS     = 30;

   cv::VideoCapture cam(CAMERA_DEVICE);
   if (!cam.isOpened()){
      std::cerr << "[ERROR] No se pudo abrir /dev/video"
                << CAMERA_DEVICE << "\n";
      std::terminate();
   }
   cam.set(cv::CAP_PROP_FRAME_WIDTH,  640.0);
   cam.set(cv::CAP_PROP_FRAME_HEIGHT, 480.0);
   cam.set(cv::CAP_PROP_FPS,     TARGET_FPS);

   Publisher pub("publisher01", BROKER_HOST, BROKER_PORT);
   pub.connect();

   int64_t frame_period_us = static_cast<int64_t>(1'000'000.0 / TARGET_FPS);
   Timer camera_timer;
   Timer metrics_timer;
   while(running)
   {
      camera_timer.start();
      publish_camera_frame(cam, pub, TOPIC_CAMERA);
      
      if (metrics_timer.ellapsed_us() > 1'000'000)
      {
         Metrics metrics = SystemMonitor::read_all_metrics();
         pub.publish(TOPIC_METRICS, metrics);
         metrics_timer.start();
      }

      camera_timer.waitUntil_us(frame_period_us);
   }
   std::cout << "\nSeñal SIGINT/SIGTERM detectada\n";
   pub.disconnect();
}


void publish_camera_frame(cv::VideoCapture& cam,
                     Publisher& pub, const char* topic )
{
   static uint32_t frame_id = 0;

   cv::Mat frame;
   if (!cam.read(frame) || frame.empty()){
      std::cerr << "[ERROR] Frame vacío, cámara desconectada?\n";
      return;
   }

   // Compresión JPEG 80% -> equilibrio calidad/tamaño
   static const std::vector<int> jpeg_params = {cv::IMWRITE_JPEG_QUALITY, 80};
   std::vector<uint8_t> jpeg_buf;
   cv::imencode(".jpg", frame, jpeg_buf, jpeg_params);
   std::size_t jpeg_buf_size = jpeg_buf.size();

   // Construir el header
   FrameHeader header {};
   header.frame_id = frame_id++;
   header.timestamp_ms = static_cast<uint32_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
         std::chrono::steady_clock::now().time_since_epoch()).count());
   header.jpeg_size = static_cast<uint32_t>(jpeg_buf_size);

   // Buffer final = header + jpeg
   std::vector<uint8_t> payload (sizeof(FrameHeader) + jpeg_buf_size);
   std::memcpy(payload.data(), &header, sizeof(FrameHeader));
   std::memcpy(payload.data() + sizeof(FrameHeader), jpeg_buf.data(), jpeg_buf_size);

   pub.publish_raw(topic, payload.data(), static_cast<int>(payload.size()));
}