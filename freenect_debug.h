#pragma once
char * debug_print_device_flags(freenect_device_flags flag);

char * get_depth_string(freenect_device * f_dev);

char * get_video_string(freenect_device * f_dev);