#pragma once
#include <cstdint>

// header que precede a cada frame JPEG
struct FrameHeader
{
   uint32_t frame_id = 0;
   uint32_t timestamp_ms = 0;
   uint32_t jpeg_size = 0;

}__attribute__((packed));