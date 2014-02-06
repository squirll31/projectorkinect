/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */
#define _CRTDBG_MAP_ALLOC


#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <libfreenect.h>
#include "freenect_debug.h"
#include <windows.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>
#include <list>
#include <string>

pthread_t freenect_thread;
volatile int die = 0;

int g_argc;
char **g_argv;

int depth_window;
int video_window;

pthread_mutex_t depth_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t video_mutex = PTHREAD_MUTEX_INITIALIZER;

// back: owned by libfreenect (implicit for depth)
// mid: owned by callbacks, "latest frame ready"
// front: owned by GL, "currently being drawn"
uint8_t *depth_mid, *depth_front;
uint8_t *rgb_back, *rgb_mid, *rgb_front;

GLuint gl_depth_tex;
GLuint gl_rgb_tex;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_led;

freenect_video_format requested_format = FREENECT_VIDEO_RGB;
freenect_video_format current_format = FREENECT_VIDEO_RGB;
freenect_resolution requested_resolution = FREENECT_RESOLUTION_HIGH;
freenect_resolution current_resolution = FREENECT_RESOLUTION_HIGH;

pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;
int got_rgb = 0;
int got_depth = 0;
int depth_on = 1;

void DispatchDraws() {
	pthread_mutex_lock(&depth_mutex);
	if (got_depth) {
		glutSetWindow(depth_window);
		glutPostRedisplay();
	}
	pthread_mutex_unlock(&depth_mutex);
	pthread_mutex_lock(&video_mutex);
	if (got_rgb) {
		glutSetWindow(video_window);
		glutPostRedisplay();
	}
	pthread_mutex_unlock(&video_mutex);
}
void DrawString(std::string * theString){
	int x = 50;
	//glColor3f(1.0, 1.0, 1.0); // Green

	glRasterPos3f(50, x, 0);

	void * font = GLUT_BITMAP_HELVETICA_18;
	unsigned int i = 0;
	for (i = 0; i < theString->length(); i++)
	{
		if (theString->at(i) != '\n')
			glutBitmapCharacter(font, theString->at(i));
		else
		{
			x += 20;
			glRasterPos3f(50, x, 0);
			glutBitmapCharacter(font, theString->at(i));
		}
	}
	delete theString;
}

void DrawDepthScene()
{
	pthread_mutex_lock(&depth_mutex);
	if (got_depth) {
		uint8_t* tmp = depth_front;
		depth_front = depth_mid;
		depth_mid = tmp;
		got_depth = 0;
	}
	pthread_mutex_unlock(&depth_mutex);

	freenect_frame_mode frame_mode = freenect_get_current_depth_mode(f_dev);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, depth_front);

	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3f(640, 0, 0);
	glTexCoord2f(1, 1); glVertex3f(640, 480, 0);
	glTexCoord2f(0, 1); glVertex3f(0, 480, 0);
	glEnd();

#if defined(_DEBUG)
	DrawString(debug_get_frame_mode_string(frame_mode));
#endif

	glutSwapBuffers();
}

void DrawVideoScene()
{
	if (requested_format != current_format || requested_resolution != current_resolution) {
		return;
	}

	pthread_mutex_lock(&video_mutex);

	freenect_frame_mode frame_mode = freenect_get_current_video_mode(f_dev);

	if (got_rgb) {
		uint8_t *tmp = rgb_front;
		rgb_front = rgb_mid;
		rgb_mid = tmp;
		got_rgb = 0;
	}

	pthread_mutex_unlock(&video_mutex);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
	if (current_format == FREENECT_VIDEO_RGB || current_format == FREENECT_VIDEO_YUV_RGB) {
		glTexImage2D(GL_TEXTURE_2D, 0, 3, frame_mode.width, frame_mode.height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_front);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, 1, frame_mode.width, frame_mode.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rgb_front);
	}
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3f(frame_mode.width, 0, 0);
	glTexCoord2f(1, 1); glVertex3f(frame_mode.width, frame_mode.height, 0);
	glTexCoord2f(0, 1); glVertex3f(0, frame_mode.height, 0);
	glEnd();

#if defined(_DEBUG)
	DrawString(debug_get_frame_mode_string(frame_mode));
#endif

	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
	if (key == 27) {
		die = 1;
		pthread_join(freenect_thread, NULL);
		glutDestroyWindow(depth_window);
		glutDestroyWindow(video_window);

		// Not pthread_exit because OSX leaves a thread lying around and doesn't exit
		exit(0);
	}
	if (key == 'f') {
		// Cycle through:
		// 1) 1280x1024 RGB
		// 2) 1280x1024 IR
		// 3) 640x480 RGB
		// 4) 640x480 YUV
		// 5) 640x480 IR
		switch (current_resolution){
		case FREENECT_RESOLUTION_HIGH:
			printf("Current resolution is HIGH\n");
			break;

		case FREENECT_RESOLUTION_LOW:
			printf("Current resolution is LOW\n");
			break;
		case FREENECT_RESOLUTION_MEDIUM:
			printf("Current resolution is MEDIUM\n");
			break;
		case FREENECT_RESOLUTION_DUMMY:
			printf("Current resolution is DUMMY?\n");
			break;
		}

		if (current_resolution == FREENECT_RESOLUTION_HIGH) {
			if (current_format == FREENECT_VIDEO_RGB) {
				requested_format = FREENECT_VIDEO_IR_8BIT;
				// Since we can't stream the high-res IR while running the depth stream,
				// we force the depth stream off when we reach this res in the cycle.
				freenect_stop_depth(f_dev);
				memset(depth_mid, 0, 640 * 480 * 3); // black out the depth camera
				got_depth++;
				depth_on = 0;
			}
			else if (current_format == FREENECT_VIDEO_IR_8BIT) {
				requested_format = FREENECT_VIDEO_RGB;
				requested_resolution = FREENECT_RESOLUTION_MEDIUM;
			}
		}
		else if (current_resolution == FREENECT_RESOLUTION_MEDIUM) {
			if (current_format == FREENECT_VIDEO_RGB) {
				requested_format = FREENECT_VIDEO_YUV_RGB;
			}
			else if (current_format == FREENECT_VIDEO_YUV_RGB) {
				requested_format = FREENECT_VIDEO_IR_8BIT;
			}
			else if (current_format == FREENECT_VIDEO_IR_8BIT) {
				requested_format = FREENECT_VIDEO_RGB;
				requested_resolution = FREENECT_RESOLUTION_HIGH;
			}
		}
		glutSetWindow(video_window);
		freenect_frame_mode s = freenect_find_video_mode(requested_resolution, requested_format);
		glutReshapeWindow(s.width, s.height);
	}
	if (key == 'd') { // Toggle depth camera.
		if (depth_on) {
			freenect_stop_depth(f_dev);
			memset(depth_mid, 0, 640 * 480 * 3); // black out the depth camera
			got_depth++;
			depth_on = 0;
		}
		else {
			freenect_start_depth(f_dev);
			depth_on = 1;
		}
	}
}

void ReSizeGLScene(int Width, int Height)
{
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Width, Height, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
}

void InitGL(int Width, int Height)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	glGenTextures(1, &gl_depth_tex);
	glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenTextures(1, &gl_rgb_tex);
	glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ReSizeGLScene(Width, Height);
}

void *gl_threadfunc(void *arg)
{
	printf("GL thread\n");

	glutInit(&g_argc, g_argv);

	// set display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

	// create depth window
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	depth_window = glutCreateWindow("Depth");

	// register depth callback
	glutDisplayFunc(&DrawDepthScene);

	// register frame dispatch callback
	glutIdleFunc(&DispatchDraws);

	// register keyboard event listener
	glutKeyboardFunc(&keyPressed);

	InitGL(640, 480);


	// get current resolution, format
	freenect_frame_mode mode = freenect_find_video_mode(current_resolution, current_format);

	// create the window
	glutInitWindowPosition(640, 0);
	glutInitWindowSize(mode.width, mode.height);
	video_window = glutCreateWindow("Video");

	// register draw function
	glutDisplayFunc(&DrawVideoScene);

	// register dispatch callback
	glutIdleFunc(&DispatchDraws);

	// register on resize function
	glutReshapeFunc(&ReSizeGLScene);

	// register key event listener
	glutKeyboardFunc(&keyPressed);

	InitGL(640, 480);

	ReSizeGLScene(mode.width, mode.height);

	// start glut thread
	glutMainLoop();

	return NULL;
}

uint16_t t_gamma[2048];


// depth sensor frame ready callback
void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	int i;
	uint16_t *depth = (uint16_t*)v_depth;

	pthread_mutex_lock(&depth_mutex);

	// convert depth to color?

	int pval;
	//int pval[640 * 480];
	//for (i = 0; i < 640 * 480; i++)
	//	pval[i] = 0;

	int lb;
	//int lb[640 * 480];
	//for (i = 0; i < 640 * 480; i++)
	//	lb[i] = 0;

	//  go through each pixel per frame.
	//    there are 307,200 pixels.
	for (i = 0; i < 640 * 480; i++) {

		//  depth: has 307,200 elements.
		//    each with a value ranging from between 0 to 2047
		//      otherwise we would be indexing out of t_gamma's range: Segmentation Fault.
		//      However, we do notice from the definition, each depth element has 16 bits available to it.
		//        we're practically dropping 5 bits from the high end of the potential 16 bits, to have 11 bits populated.

		//  then we use the resulting 11 bit unsigned integer to index into t_gamma, 
		//    and get an unsigned 16 bit integer, and cast it to a regular integer.
		pval = t_gamma[depth[i]];
		// pval[i] = t_gamma[depth[i]];

		//  we take our previous unsigned 16 bit integer, cast to integer,
		//    and take only the bottom 8 bits.
		//      call it lb.
		lb = pval & 0xff;
		/*lb[i] = pval[i] & 0xff;*/

		//  we take our previous unsigned 16 bit integer, cast to integer,
		//    and shift its bits to the right 8?
		//      which is the equivalent of dividing by 256?
		//        and truncates, because switch only takes integers.
		//          which leaves us only the high 8 bits of t_gamma[depth[i]]
		switch (pval >> 8) {
		//switch (pval[i] >> 8) {
		case 0:
			depth_mid[3 * i + 0] = 255;
			depth_mid[3 * i + 1] = 255 - lb;
			depth_mid[3 * i + 2] = 255 - lb;
			break;
		case 1:
			depth_mid[3 * i + 0] = 255;
			depth_mid[3 * i + 1] = lb;
			depth_mid[3 * i + 2] = 0;
			break;
		case 2:
			depth_mid[3 * i + 0] = 255 - lb;
			depth_mid[3 * i + 1] = 255;
			depth_mid[3 * i + 2] = 0;
			break;
		case 3:
			depth_mid[3 * i + 0] = 0;
			depth_mid[3 * i + 1] = 255;
			depth_mid[3 * i + 2] = lb;
			break;
		case 4:
			depth_mid[3 * i + 0] = 0;
			depth_mid[3 * i + 1] = 255 - lb;
			depth_mid[3 * i + 2] = 255;
			break;
		case 5:
			depth_mid[3 * i + 0] = 0;
			depth_mid[3 * i + 1] = 0;
			depth_mid[3 * i + 2] = 255 - lb;
			break;
		default:
			depth_mid[3 * i + 0] = 0;
			depth_mid[3 * i + 1] = 0;
			depth_mid[3 * i + 2] = 0;
			break;
		}
	}

	//char filename[128];

	//FILE* depth_FILE = NULL;
	//sprintf_s(filename, 128, "bin\\Depth_FrameDump\\depth.%032u.hex", timestamp);
	//fopen_s(&depth_FILE, filename, "w+");

	//FILE* pval_FILE = NULL;
	//sprintf_s(filename, 128, "bin\\Depth_FrameDump\\pval.%032u.hex", timestamp);
	//fopen_s(&pval_FILE, filename, "w+");

	//FILE* lb_FILE = NULL;
	//sprintf_s(filename, 128, "bin\\Depth_FrameDump\\lb.%032u.hex", timestamp);
	//fopen_s(&lb_FILE, filename, "w+");

	//FILE* depthMID_FILE = NULL;
	//sprintf_s(filename, 128, "bin\\Depth_FrameDump\\depthMID.%032u.hex", timestamp);
	//fopen_s(&depthMID_FILE, filename, "w+");

	//if ((depth_FILE != NULL) && (pval_FILE != NULL) && (lb_FILE != NULL) && (depthMID_FILE != NULL)) {
	//	for (i = 0; i < 640 * 480; i++) {
	//		//  depth is uint16_t
	//		fprintf(depth_FILE, "depth[%06i] = %#010X %#010X\n", i, depth[i] & 0xFF00, depth[i] & 0x00FF);
	//		fprintf(pval_FILE, "pval[%06i] = %#010X %#010X\n", i, pval & 0xFF00, pval & 0x00FF);
	//		fprintf(lb_FILE, "lb[%06i] = %#010X %#010X\n", i, lb & 0xFF00, lb & 0x00FF);
	//		fprintf(depthMID_FILE, "depth_mid[%06i] = %#010X\n", 3 * i + 0, depth_mid[3 * i + 0]);
	//		fprintf(depthMID_FILE, "depth_mid[%06i] = %#010X\n", 3 * i + 1, depth_mid[3 * i + 1]);
	//		fprintf(depthMID_FILE, "depth_mid[%06i] = %#010X\n", 3 * i + 2, depth_mid[3 * i + 2]);
	//	}
	//	fclose(depth_FILE);
	//	fclose(pval_FILE);
	//	fclose(lb_FILE);
	//	fclose(depthMID_FILE);
	//}

	got_depth++;
	pthread_mutex_unlock(&depth_mutex);
}

void video_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	pthread_mutex_lock(&video_mutex);

	// swap buffers
	assert(rgb_back == rgb);
	rgb_back = rgb_mid;
	freenect_set_video_buffer(dev, rgb_back);
	rgb_mid = (uint8_t*)rgb;

	got_rgb++;
	pthread_mutex_unlock(&video_mutex);
}

void *freenect_threadfunc(void *arg)
{
	// set led color
	freenect_set_led(f_dev, LED_RED);

	// register callback for depth frame
	freenect_set_depth_callback(f_dev, depth_cb);
	// register callback for video frame
	freenect_set_video_callback(f_dev, video_cb);

	// set a resolution for the video camera
	// default is "HIGH", "RGB"
	freenect_set_video_mode(f_dev, freenect_find_video_mode(current_resolution, current_format));

	// set a resolution for the depth sensor
	//FREENECT_RESOLUTION_LOW = 0, /**< QVGA - 320x240 */
	//	FREENECT_RESOLUTION_MEDIUM = 1, /**< VGA  - 640x480 */
	//	FREENECT_RESOLUTION_HIGH = 2, /**< SXGA - 1280x1024 */
	freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));

	// allocate video buffers
	rgb_back = (uint8_t*)malloc(freenect_find_video_mode(current_resolution, current_format).bytes);
	rgb_mid = (uint8_t*)malloc(freenect_find_video_mode(current_resolution, current_format).bytes);
	rgb_front = (uint8_t*)malloc(freenect_find_video_mode(current_resolution, current_format).bytes);

	// set buffer for video camera
	freenect_set_video_buffer(f_dev, rgb_back);

	// start depth and video
	freenect_start_depth(f_dev);
	freenect_start_video(f_dev);

	int status = 0;

	// frame loop
	while (!die && status >= 0) {
		// communicate via usb
		status = freenect_process_events(f_ctx);

		// change resolution on user input
		if (requested_format != current_format || requested_resolution != current_resolution) {
			// stop video
			freenect_stop_video(f_dev);
			// change kinect video mode
			freenect_set_video_mode(f_dev, freenect_find_video_mode(requested_resolution, requested_format));

			// lock the video mutex so the video callback doesn't try to write
			pthread_mutex_lock(&video_mutex);

			// free the video buffers
			free(rgb_back);
			free(rgb_mid);
			free(rgb_front);

			// malloc the video buffers
			rgb_back = (uint8_t*)malloc(freenect_find_video_mode(requested_resolution, requested_format).bytes);
			rgb_mid = (uint8_t*)malloc(freenect_find_video_mode(requested_resolution, requested_format).bytes);
			rgb_front = (uint8_t*)malloc(freenect_find_video_mode(requested_resolution, requested_format).bytes);

			current_format = requested_format;
			current_resolution = requested_resolution;
			pthread_mutex_unlock(&video_mutex);

			// reassign the video buffer
			freenect_set_video_buffer(f_dev, rgb_back);

			// resume video
			freenect_start_video(f_dev);
		}
	}

	// in case of error
	if (status < 0) {
		printf("Something went terribly wrong.  Aborting.\n");
		return NULL;
	}

	printf("\nshutting down streams...\n");

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);
	pthread_mutex_lock(&depth_mutex);
	free(depth_mid);
	free(depth_front);
	pthread_mutex_unlock(&depth_mutex);
	pthread_mutex_lock(&video_mutex);
	free(rgb_back);
	free(rgb_mid);
	free(rgb_front);
	pthread_mutex_unlock(&video_mutex);
	printf("-- done!\n");
	return NULL;
}

int main(int argc, char **argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int res;

	//  if we are dumping depth frames...
	//  make the directories needed to contain them.
	//  first. let us get the present working directory.
	//unsigned int pwd_len = GetCurrentDirectory(0, NULL);
	//TCHAR* pwd = new wchar_t[pwd_len];
	//GetCurrentDirectory(pwd_len, pwd);

	// TCHAR* depthFrameDIR = new wchar_t[pwd_len + wcslen(L"\\DepthFrameDumps\\")];
	// strcat(depthFrameDIR, );

	////  second. let us check to see if our intermediate directories exists
	//if (!CreateDirectory(L"\\DepthFrameDumps", NULL))
	//{
	//	//  the directory existed.

	//}
	//else { /* the directory was made. */ }

	//  third. then let us make it if they didn't already exist.
	//  fourth. 

	// allocate 8-bit uint*s to hold depth data
	//depth_mid = new uint8_t[640 * 480 * 3];
	//depth_front = new uint8_t[640 * 480 * 3];

	depth_mid = (uint8_t*)malloc(640 * 480 * 3);
	depth_front = (uint8_t*)malloc(640 * 480 * 3);

	printf("Kinect camera test\n");

	int i;
	for (i = 0; i < 2048; i++) {
		float v = i / 2048.0;
		v = powf(v, 3) * 6;
		t_gamma[i] = v * 6 * 256;
		//printf("t_gamma[%04i] = %i\n", i, t_gamma[i]);
		//printf("t_gamma[%04i] = 0x%08X 0x%08X.\n", i, t_gamma[i] & 0x00FF, t_gamma[i] & 0xFF00);
		//  t_gamma[0] = 0
		//  t_gamma[   1] = 6 * (6 * (1 / 2^33)) * 256
		//  t_gamma[   1] = 9 / 2^23 = ??? small number. much below 1.
		//  t_gamma[   2] = 6 * (6 * (1 / 2^30)) * 256
		//  t_gamma[   3] = 6 * (6 * (9 / 2^33)) * 256
		//  t_gamma[   4] = 6 * (6 * (1 / 2^9 )) * 256
		//  t_gamma[2048] = 6 * (6 * (2048 / 2048)^3 * 256
		//  t_gamma[2048] = 36 * 2^8
		//  t_gamma[2048] = 9 * 2^10
		//  t_gamma[2048] = ...about 9K?
	}

	//FILE* pFILE = NULL;

	//fopen_s(&pFILE, "t_gamma.integer", "w+");
	//if (pFILE != NULL) {
	//	for (i = 0; i < 2048; i++)
	//		// printf("t_gamma[%04i] = %i\n", i, t_gamma[i]);
	//		fprintf(pFILE, "t_gamma[%04i] = %i\n", i, t_gamma[i]);
	//	fclose(pFILE);
	//}

	//fopen_s(&pFILE, "t_gamma.hex", "w+");
	//if (pFILE != NULL) {
	//	for (i = 0; i < 2048; i++)
	//		// printf("t_gamma[%04i] = 0x%08X 0x%08X.\n", i, t_gamma[i] & 0xFF00, t_gamma[i] & 0x00FF);
	//		fprintf(pFILE, "t_gamma[%04i] = 0x%08X 0x%08X.\n", i, t_gamma[i] & 0xFF00, t_gamma[i] & 0x00FF);
	//	fclose(pFILE);
	//}

	g_argc = argc;
	g_argv = argv;

	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return 1;
	}

	// set up logging
	freenect_set_log_level(f_ctx, FREENECT_LOG_INFO);

	// select which kinect parts to open
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

	// enumerate kinects attached
	int nr_devices = freenect_num_devices(f_ctx);
	printf("Number of devices found: %d\n", nr_devices);

	// argv[1] is which device to use, default 0
	int user_device_number = 0;
	if (argc > 1)
		user_device_number = atoi(argv[1]);

	// if no device is attached, quit
	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
		return 1;
	}

	// open device or die
	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	// start the freenect thread
	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		freenect_shutdown(f_ctx);
		return 1;
	}

	// OS X requires GLUT to run on the main thread
	gl_threadfunc(NULL);


	pthread_mutex_lock(&depth_mutex);
	free(depth_mid);
	free(depth_front);
	pthread_mutex_unlock(&depth_mutex);
	pthread_mutex_lock(&video_mutex);
	free(rgb_back);
	free(rgb_mid);
	free(rgb_front);
	pthread_mutex_unlock(&video_mutex);


	_CrtDumpMemoryLeaks();
	return 0;
}
