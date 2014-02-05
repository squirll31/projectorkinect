#pragma once
#include <libfreenect.h>
#include <string>


// @group threads
void *gl_threadfunc(void *arg);

// @group threads
void *freenect_threadfunc(void *arg);

// @group gl
void DispatchDraws();
void ReSizeGLScene(int Width, int Height);

void InitGL(int Width, int Height);


// @group Render
// draw depth scene (called after depth_cb)
void DrawString(std::string * theString);

// @group Render
// draw depth scene (called after depth_cb)
// render depth sensor frame
void DrawDepthScene();

// @group Render
// render video frame (called after video_cb)
void DrawVideoScene();

// @ group callbacks
// freenect depth callback. "on frame ready"
void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp);

// @ group callbacks
// freenect video callback. "frame is ready"
void video_cb(freenect_device *dev, void *rgb, uint32_t timestamp);

// @group  callbacks
// GLUT keypress callback. "keyboard input ready"
void keyPressed(unsigned char key, int x, int y);

void cleanup();