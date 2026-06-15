#pragma once
#include <cstdint>
#include <opencv2/opencv.hpp>

#include "thread_safe_object.hpp"
#include "shared_frame.hpp"
#include "thread_man.hpp"


class CameraThread: public ThreadMan
{
   ThreadSafeObject<SharedFrame>& shared_buff; 

   // Image processing
   void run() override
   {
      cv::namedWindow("Camera (rotated 180)", cv::WINDOW_AUTOSIZE);

      while(m_running){
         SharedFrame frame {};
         // Si tardamos mucho en recibir un nuevo frame esperamos
         shared_buff.wait_and_get(frame);

         // Decodificar JPEG
         cv::Mat jpeg_buf(1, static_cast<int>(frame.jpeg_data.size()), 
                          CV_8UC1, frame.jpeg_data.data());

         cv::Mat cv_frame = cv::imdecode(jpeg_buf, cv::IMREAD_COLOR);
         if (cv_frame.empty()){
            std::cerr << "[ERROR] No se pudo decodificar el frame "
                      << frame.frame_id << "\n";
            continue;
         }
         // Rotacion 180º
         cv::Mat rotated;
         cv::rotate(cv_frame, rotated, cv::ROTATE_180);
         cv::imshow("Camera (rotated 180)", rotated);
         cv::waitKey(1);

         // std::cout << "Processing frame " << frame.frame_id << "\n";
      }
      cv::destroyAllWindows();
   }

public:
   CameraThread(ThreadSafeObject<SharedFrame>& buff)
      : shared_buff(buff) {}
   ~CameraThread() { stop(); }
};