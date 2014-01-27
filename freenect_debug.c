#include "libfreenect.h"
#include "freenect_debug.h"
#include <stdio.h>

#define _tostr(a) #a
#define tostr(a) _tostr(a)


char * debug_print_device_flags(freenect_device_flags flag){
	char * h;
	if (flag & FREENECT_DEVICE_MOTOR)
	{
		h = "FREENECT_DEVICE_MOTOR";
	}
	if (flag & FREENECT_DEVICE_CAMERA)
	{
		h = "FLAG_DEVICE_MOTOR";
	}
	if (flag & FREENECT_DEVICE_AUDIO)
	{
		h = "FLAG_DEVICE_AUDIO";
	}
	return h;
}

char * debug_get_depth_string(freenect_depth_format depth_format){
	char * format;
	char * out;
	switch (depth_format){
	case FREENECT_DEPTH_11BIT:
		format = tostr(FREENECT_DEPTH_11BIT);
		break;
	case FREENECT_DEPTH_11BIT_PACKED:
		format = tostr(FREENECT_DEPTH_11BIT_PACKED);
		break;
	case FREENECT_DEPTH_10BIT:
		format = tostr(FREENECT_DEPTH_10BIT);
		break;
	case FREENECT_DEPTH_10BIT_PACKED:
		format = tostr(FREENECT_DEPTH_10BIT_PACKED);
		break;
	case FREENECT_DEPTH_DUMMY:
		format = tostr(FREENECT_DEPTH_DUMMY);
		break;
	case FREENECT_DEPTH_REGISTERED:
		format = tostr(FREENECT_DEPTH_REGISTERED);
		break;
	case FREENECT_DEPTH_MM:
		format = tostr(FREENECT_DEPTH_MM);
		break;
	default:
		format = "unknown";
	}
	int strsize = strlen(format) + 5;
	out = new char[strsize];
	strcpy(out, format);

	return out;
}

char * debug_get_video_string(freenect_video_format  video_format){

	char * format;
	char * out;
	switch (video_format){
	case FREENECT_VIDEO_RGB:
		format = tostr(FREENECT_VIDEO_RGB);
		break;
	case FREENECT_VIDEO_BAYER:
		format = tostr(FREENECT_VIDEO_BAYER);
		break;
	case FREENECT_VIDEO_IR_8BIT:
		format = tostr(FREENECT_VIDEO_IR_8BIT);
		break;
	case FREENECT_VIDEO_IR_10BIT:
		format = tostr(FREENECT_VIDEO_IR_10BIT);
		break;
	case FREENECT_VIDEO_IR_10BIT_PACKED:
		format = tostr(FREENECT_VIDEO_IR_10BIT_PACKED);
		break;
	case FREENECT_VIDEO_YUV_RGB:
		format = tostr(FREENECT_VIDEO_YUV_RGB);
		break;
	case FREENECT_VIDEO_YUV_RAW:
		format = tostr(FREENECT_VIDEO_YUV_RAW);
		break;
	case FREENECT_VIDEO_DUMMY:
		format = tostr(FREENECT_VIDEO_DUMMY);
		break;
	default:
		format = "unknown";
	}
	int strsize = strlen(format) + 5;
	out = new char[strsize];
	strcpy(out, format);

	return out;
}

char * debug_get_resolution(freenect_resolution res){
	char * resolution;
	char * out;
	switch (res){
	case FREENECT_RESOLUTION_LOW:
		resolution = tostr(FREENECT_RESOLUTION_LOW);
		break;
	case FREENECT_RESOLUTION_MEDIUM:
		resolution = tostr(FREENECT_RESOLUTION_MEDIUM);
		break;
	case FREENECT_RESOLUTION_HIGH:
		resolution = tostr(FREENECT_RESOLUTION_HIGH);
		break;
	case FREENECT_RESOLUTION_DUMMY:
		resolution = tostr(FREENECT_RESOLUTION_DUMMY);
		break; 
	default:
		resolution = "unknown";
	}
	int strsize = strlen(resolution) + 5;
	out = new char[strsize];
	strcpy(out, resolution);
	return out;
}

char * debug_get_frame_mode_string(freenect_frame_mode frame_mode){
	char * out = new char[2048];
	int out_size = 0;
	char * resolution = debug_get_resolution(frame_mode.resolution);
	char * video_format = debug_get_video_string(frame_mode.video_format);
	char * video_format_enum = new char[25];
	char * depth_format = debug_get_depth_string(frame_mode.depth_format);
	char * depth_format_enum = new char[25];
	char * bytes = new char[25];
	char * width = new char[25];
	char * height = new char[25];
	char * data_bits_per_pixel = new char[25];
	char * padding_bits_per_pixel = new char[25];
	char * framerate = new char[25];
	char * is_valid = new char[6];
	if (frame_mode.is_valid == 0){
		strcpy(is_valid, "FALSE");
	}
	else {
		strcpy(is_valid, "TRUE");
	}
	
	sprintf(out, "freenect_resolution[%i] = %s\nfreenect_video_format[%i] = %s\nfreenect_depth_format[%i] = %s\nWidth: %i px\nHeight: %i px\nBuffer Size Per Frame: %i bytes\nData Bits Per Pixel: %i bits\nPadding Bits Per Pixel: %i bits\nFramerate: %i Hz\nIs Valid: %s\n", frame_mode.resolution, resolution,  frame_mode.video_format,video_format,  frame_mode.depth_format, depth_format, frame_mode.width, frame_mode.height, frame_mode.bytes, frame_mode.data_bits_per_pixel, frame_mode.padding_bits_per_pixel, frame_mode.framerate, is_valid);
	return out;
}