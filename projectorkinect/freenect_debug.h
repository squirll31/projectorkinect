#pragma once
#include <string>

#define DEBUG_VIDEO_FORMATS_MAX 7
#define DEBUG_DEPTH_FORMATS_MAX 6

//freenect_video_format debug_video_formats[DEBUG_VIDEO_FORMATS_MAX] = { FREENECT_VIDEO_RGB, FREENECT_VIDEO_BAYER,
//FREENECT_VIDEO_IR_8BIT, FREENECT_VIDEO_IR_10BIT,
//FREENECT_VIDEO_IR_10BIT_PACKED, FREENECT_VIDEO_YUV_RGB,
//FREENECT_VIDEO_YUV_RAW };
//
//
//freenect_depth_format debug_depth_formats[DEBUG_DEPTH_FORMATS_MAX] = { FREENECT_DEPTH_11BIT,
//FREENECT_DEPTH_10BIT,
//FREENECT_DEPTH_11BIT_PACKED,
//FREENECT_DEPTH_10BIT_PACKED,
//FREENECT_DEPTH_REGISTERED,
//FREENECT_DEPTH_MM };

std::string * debug_get_depth_string(freenect_depth_format depth_mode);

std::string * debug_get_video_string(freenect_video_format video_mode);

std::string * debug_get_frame_mode_string(freenect_frame_mode frame_mode);

std::string * debug_get_resolution_string(freenect_resolution res);
