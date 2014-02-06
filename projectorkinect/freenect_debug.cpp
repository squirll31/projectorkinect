#include <libfreenect.h>
#include "freenect_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#define _tostr(a) #a
#define tostr(a) _tostr(a)


std::string * debug_get_depth_string(freenect_depth_format depth_format){
	std::string * format;
	switch (depth_format){
	case FREENECT_DEPTH_11BIT:
		format = new std::string(tostr(FREENECT_DEPTH_11BIT));
		break;
	case FREENECT_DEPTH_11BIT_PACKED:
		format = new std::string(tostr(FREENECT_DEPTH_11BIT_PACKED));
		break;
	case FREENECT_DEPTH_10BIT:
		format = new std::string(tostr(FREENECT_DEPTH_10BIT));
		break;
	case FREENECT_DEPTH_10BIT_PACKED:
		format = new std::string(tostr(FREENECT_DEPTH_10BIT_PACKED));
		break;
	case FREENECT_DEPTH_DUMMY:
		format = new std::string(tostr(FREENECT_DEPTH_DUMMY));
		break;
	case FREENECT_DEPTH_REGISTERED:
		format = new std::string(tostr(FREENECT_DEPTH_REGISTERED));
		break;
	case FREENECT_DEPTH_MM:
		format = new std::string(tostr(FREENECT_DEPTH_MM));
		break;
	default:
		format = new std::string(tostr("Unknown"));
	}

	return format;
}

std::string * debug_get_video_string(freenect_video_format  video_format){
	std::string * format;
	switch (video_format){
	case FREENECT_VIDEO_RGB:
		format = new std::string(tostr(FREENECT_VIDEO_RGB));
		break;
	case FREENECT_VIDEO_BAYER:
		format = new std::string(tostr(FREENECT_VIDEO_BAYER));
		break;
	case FREENECT_VIDEO_IR_8BIT:
		format = new std::string(tostr(FREENECT_VIDEO_IR_8BIT));
		break;
	case FREENECT_VIDEO_IR_10BIT:
		format = new std::string(tostr(FREENECT_VIDEO_IR_10BIT));
		break;
	case FREENECT_VIDEO_IR_10BIT_PACKED:
		format = new std::string(tostr(FREENECT_VIDEO_IR_10BIT_PACKED));
		break;
	case FREENECT_VIDEO_YUV_RGB:
		format = new std::string(tostr(FREENECT_VIDEO_YUV_RGB));
		break;
	case FREENECT_VIDEO_YUV_RAW:
		format = new std::string(tostr(FREENECT_VIDEO_YUV_RAW));
		break;
	case FREENECT_VIDEO_DUMMY:
		format = new std::string(tostr(FREENECT_VIDEO_DUMMY));
		break;
	default:
		format = new std::string("Unknown");
	}
	return format;
}

std::string * debug_get_resolution_string(freenect_resolution res){
	std::string * resolution;
	switch (res){
	case FREENECT_RESOLUTION_LOW:
		resolution = new std::string(tostr(FREENECT_RESOLUTION_LOW));
		break;
	case FREENECT_RESOLUTION_MEDIUM:
		resolution = new std::string(tostr(FREENECT_RESOLUTION_MEDIUM));
		break;
	case FREENECT_RESOLUTION_HIGH:
		resolution = new std::string(tostr(FREENECT_RESOLUTION_HIGH));
		break;
	case FREENECT_RESOLUTION_DUMMY:
		resolution = new std::string(tostr(FREENECT_RESOLUTION_DUMMY));
		break;
	default:
		resolution = new std::string("Unknown");
	}
	return resolution;
}

std::string * debug_get_frame_mode_string(freenect_frame_mode frame_mode){

	std::string * frame_mode_string = new std::string();
	std::string * res_string = debug_get_resolution_string(frame_mode.resolution);
	std::string * vid_string = debug_get_video_string(frame_mode.video_format);
	std::string * dep_string = debug_get_depth_string(frame_mode.depth_format);

	frame_mode_string->append("freenect_resolution[");
	frame_mode_string->append(std::to_string(frame_mode.resolution));
	frame_mode_string->append("] = ");
	frame_mode_string->append(res_string->c_str());

	frame_mode_string->append("\nfreenect_video_Format[");
	frame_mode_string->append(std::to_string(frame_mode.video_format));
	frame_mode_string->append("] = ");
	frame_mode_string->append(vid_string->c_str());

	frame_mode_string->append("\nfreenect_depth_format[");
	frame_mode_string->append(std::to_string(frame_mode.depth_format));
	frame_mode_string->append("] = ");
	frame_mode_string->append(dep_string->c_str());

	frame_mode_string->append("\nwidth = ");
	frame_mode_string->append(std::to_string(frame_mode.width));
	frame_mode_string->append(" px");

	frame_mode_string->append("\nheight = ");
	frame_mode_string->append(std::to_string(frame_mode.height));
	frame_mode_string->append(" px");

	frame_mode_string->append("\nbuffer size per frame = ");
	frame_mode_string->append(std::to_string(frame_mode.bytes));
	frame_mode_string->append(" bytes");

	frame_mode_string->append("\ndata bits per pixel = ");
	frame_mode_string->append(std::to_string(frame_mode.data_bits_per_pixel));
	frame_mode_string->append(" bits");

	frame_mode_string->append("\npadding bits per pixel = ");
	frame_mode_string->append(std::to_string(frame_mode.padding_bits_per_pixel));
	frame_mode_string->append(" bits");

	frame_mode_string->append("\nframe rate = ");
	frame_mode_string->append(std::to_string(frame_mode.framerate));
	frame_mode_string->append(" Hz");

	frame_mode_string->append("\nis valid = ");
	if (frame_mode.is_valid == 0){
		frame_mode_string->append("TRUE");
	}
	else {
		frame_mode_string->append("FALSE");
	}

	delete vid_string;
	delete dep_string;
	delete res_string;
	return frame_mode_string;
}