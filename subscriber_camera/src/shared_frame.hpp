#pragma once
#include <vector>
#include <cstdint>

struct SharedFrame 
{
   uint32_t             frame_id    {0};
   std::vector<uint8_t> jpeg_data   {};
};