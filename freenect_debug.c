#include "libfreenect.h"
#include "freenect_debug.h"

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
	char * num = new char[25];
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
	strcpy(num, itoa(depth_format, num, 10));
	int strsize = strlen(num) + strlen(format) + 5;
	out = new char[strsize];
	strcpy(out, format);

	strcat(out, " - ");
	strcat(out, num);
	return out;
}

char * debug_get_video_string(freenect_video_format  video_format){

	char * format;
	char * num = new char[25];
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
	strcpy(num, itoa(video_format, num, 10));
	int strsize = strlen(num) + strlen(format) + 5;
	out = new char[strsize];
	strcpy(out, format);

	strcat(out, " - ");
	strcat(out, num);
	return out;
}