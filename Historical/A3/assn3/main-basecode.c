/*
Base code for COMP-557 Assignment #3
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <values.h>
#include <GL/glut.h>
#include <assert.h>
#include "XYZ.h"

#define ESC '\033'
#define PI 3.1457
#define FOV    30	// field of veiw

/* creates a enum type for mouse buttons */
enum {
    BUTTON_LEFT = 0,
    BUTTON_RIGHT,
};

/* set global variables */
int mButton = -1;
int mOldY, mOldX;
/* XYZs that for translation & rotation of the camera */
XYZ eye = {0.0f, 0.0f, 5.0f};
XYZ lookat = {0.0f, 0.0f, 0.0f};
XYZ updir = {0.0f, 1.0f, 0.0f};
XYZ rot = {0.0f, 0.0f, 0.0f};
XYZ viewDir = {0.0f, 0.0f, 0.0f};

/* windows size and position constants */
int GL_WIN_WIDTH  = 640;
int GL_WIN_HEIGHT = 480;
int GL_WIN_INITIAL_X = 0;
int GL_WIN_INITIAL_Y = 0;	


/* texture variables */
unsigned int texID;
unsigned char* texture;

#define BBOX_SIZE 8	

XYZ theBbox[] = {	// the vertices of the bounding box/cube
	{0, 0, 0},
	{0, 0, 1},
	{0, 1, 0},
	{1, 0, 0},
	{0, 1, 1},
	{1, 0, 1},
	{1, 1, 0},
	{1, 1, 1}
};

struct Line {
	XYZ p1;
	XYZ p2;
};

typedef struct Line Line;

#define BBOX_LINE_COUNT 12

Line theBboxLines[] = {		// the lines of the bounding box
	{{0, 0, 0}, {0, 0, 1}},
	{{0, 0, 0}, {0, 1, 0}},
	{{0, 0, 0}, {1, 0, 0}},
	{{0, 1, 0}, {1, 1, 0}},
	{{0, 0, 1}, {0, 1, 1}},
	{{0, 0, 1}, {1, 0, 1}},
	{{0, 1, 0}, {0, 1, 1}},
	{{0, 1, 1}, {1, 1, 1}},
	{{1, 0, 0}, {1, 0, 1}},
	{{1, 0, 0}, {1, 1, 0}},
	{{1, 0, 1}, {1, 1, 1}},
	{{1, 1, 0}, {1, 1, 1}}
};

// TEXTURE DEFINITIONS

#define WIDTH 4
#define HEIGHT 4
#define DEPTH 4
#define OPACITY  10	// opacity = alpha for our texture

#define BYTES_PER_TEXEL 4
#define LAYER(r) (WIDTH * HEIGHT * r * BYTES_PER_TEXEL)
// 2->1 dimension mapping function
#define TEXEL2(s, t) (BYTES_PER_TEXEL * (s * WIDTH + t))
// 3->1 dimension mapping function
#define TEXEL3(s, t, r)	(TEXEL2(s, t) + LAYER(r))

#define NUMSLICES  1

/* function prototypes */
void glutResize( int w, int h);
void clamp(XYZ v);
void glutMotion(int x, int y);
void glutMouse(int button, int state, int x, int y);
void glutDisplay(void);
void InitializeOGL();
void setUpTextures(char* fileName);
void drawVolume(XYZ viewDir);
unsigned char* buildTexture();
void drawBoundingBox();

//------------------------------------------------------------------------
// Window resize function
//------------------------------------------------------------------------

void glutResize(int width, int height) {
	GL_WIN_WIDTH = width;
	GL_WIN_HEIGHT = height;
  	glViewport(0, 0, width, height);
  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
  	
	/* modify this line to change perspective values */
  	gluPerspective(FOV, (double)width/(double)height, 0.1, 100.0);
  	glMatrixMode(GL_MODELVIEW);
  	glLoadIdentity();
}


//------------------------------------------------------------------------
// If rotation angle is greater of 360 or lesser than -360,
// reset angle back to zero.
//------------------------------------------------------------------------

void clamp(XYZ v) {
  	if (v.x > 360 || v.x < -360)
      		v.x = 0;
	if (v.y > 360 || v.y < -360)
      		v.y = 0;
	if (v.z > 360 || v.z < -360)
      		v.z = 0;
}

//------------------------------------------------------------------------
// Moves the camera based on mouse pressed button
//------------------------------------------------------------------------
void glutMotion(int x, int y) {
  if (mButton == BUTTON_LEFT) {

    //  Rotates each object in the world
    //	NB: this will change the vertices of your cube in the world.
    //	You should  account for this in your drawVolume() code.

	rot.x -= (mOldY - y);
	rot.y -= (mOldX - x);
	clamp (rot);
  }
  else if (mButton == BUTTON_RIGHT) {

    //	Moves the eye/camera.  This will change the view direction.
    //	See glutDisplay().

    	eye.x += (mOldX - x) * 0.01f;
    	eye.y -= (mOldY - y) * 0.01f;
  }

  mOldX = x;
  mOldY = y;
  
  glutPostRedisplay();
}

//------------------------------------------------------------------------
// Function that handles mouse input
//------------------------------------------------------------------------
void glutMouse(int button, int state, int x, int y) {
  if(state == GLUT_DOWN) {
    mOldX = x;
    mOldY = y;
    switch(button) {
    case GLUT_LEFT_BUTTON:
	mButton = BUTTON_LEFT;
	break;
    case GLUT_RIGHT_BUTTON:
      mButton = BUTTON_RIGHT;
      break;
    }
 }
  else if (state == GLUT_UP)
      mButton = -1;
}

//************************************************************************
// The glutDisplay is responsible for drawing to the window and is called
// whenever the window needs to be redrawn. 
// Therefore, this is where we call our draw routine (i.e. drawSolarSystem). 
//************************************************************************
void glutDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode( GL_MODELVIEW );
    	glLoadIdentity();
    	gluLookAt( eye.x, eye.y, eye.z,
	       lookat.x, lookat.y, lookat.z,
	       updir.x, updir.y, updir.z );
	
	// Rotate each object in the world by the angles provided by rot XYZ
	// this is only done when rotating using the left mouse button
	glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
	
	// compute the viewing direction
	viewDir.x = lookat.x - eye.x;
	viewDir.y = lookat.y - eye.y;
	viewDir.z = lookat.z - eye.z;
	
	// normalize 
	double len;
    	len = sqrt( viewDir.x*viewDir.x + viewDir.y*viewDir.y + viewDir.z*viewDir.z );
    	viewDir.x = viewDir.x/len;
	viewDir.y = viewDir.y/len;
	viewDir.z = viewDir.z/len;
	
	drawBoundingBox();
    	drawVolume(viewDir); 
	
	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------------------
// The code here can be freely altered too. It's mainly designed to set
// OpenGl's initial value and configuration.
//----------------------------------------------------------------------------------------
void InitializeOGL() {
	glClearColor(0,0,0,0);	// clear colour is black
	glColor4d(0.0, 0.0, 0.0, 1.0);
	glShadeModel(GL_FLAT);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // have to clear color & depth
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // our texture colors will replace the untextured colors

	texture = buildTexture();
}


//----------------------------------------------------------------------------------------
// This function builds a 3d 4x4 textures
// (based on code from http://gpwiki.org/index.php/OpenGL_3D_Textures)
//----------------------------------------------------------------------------------------
unsigned char* buildTexture() {

	// allocate memory for our texture
	unsigned char *texels = (unsigned char *)malloc(WIDTH * HEIGHT * DEPTH * BYTES_PER_TEXEL);
	if (texels == NULL)
		return NULL;
	int s, t;

	// Each of the following loops defines one layer of our 3d texture (texture is 4x4x4)
	// There are 4 unsigned bytes (red, green, blue, alpha) for each texel so each iteration sets 4 bytes
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 0)] =   128;
			texels[TEXEL3(s, t, 0)+1] = 0;
			texels[TEXEL3(s, t, 0)+2] = 0;
			texels[TEXEL3(s, t, 0)+3] = OPACITY;
		}
	}
	// define layer 1 as green
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 1)] =   0;
			texels[TEXEL3(s, t, 1)+1] = 128;
			texels[TEXEL3(s, t, 1)+2] = 0;
			texels[TEXEL3(s, t, 1)+3] = OPACITY;
		}
	}
	// define layer 2 as blue
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 2)] = 0;
			texels[TEXEL3(s, t, 2)+1] = 0;
			texels[TEXEL3(s, t, 2)+2] = 128;
			texels[TEXEL3(s, t, 2)+3] = OPACITY;
		}
	}

	// define layer 3 as magenta
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 3)] =   128;
			texels[TEXEL3(s, t, 3)+1] = 0;
			texels[TEXEL3(s, t, 3)+2] = 128;
			texels[TEXEL3(s, t, 3)+3] = OPACITY;
		}
	}

	// assign opacity 1 to one vertex to create a "solid mass" in the volume
        texels[TEXEL3(WIDTH/2, HEIGHT/2, 1)+3] = 255;

	// request texture id from OpenGL
	glGenTextures(1, &texID);	
	// tell OpenGL we're going to be setting up the texture id it gave us	
	glBindTexture(GL_TEXTURE_3D, texID);	
	// when this texture needs to be shrunk to fit on small polygons, use linear interpolation of the texels to determine the color
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// when this texture needs to be magnified to fit on a big polygon, use linear interpolation of the texels to determine the color
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// we want the texture to repeat over the S axis, so if we specify coordinates out of range we still get textured.
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// same as above for T axis
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// same as above for R axis
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	// this is a 3d texture, level 0 (max detail), GL should store it in RGBA8 format, its WIDTHxHEIGHTxDEPTH in size, 
	// it doesnt have a border, we're giving it to GL in RGBA format as a series of unsigned bytes, and texels is where the texel data is.
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);

	return texels;
}


//************************************************************************
// THIS IS WHERE YOU SHOULD WRITE YOUR VOLUME RENDERING CODE
// Where you will compute a series of polygons that cut through the data
// perpendicular to the direction of view, render each slice as a textured 
// polygon, from back to front and blend the slices together
// (of course you may want to use helper functions) 
//************************************************************************

void drawVolume(XYZ viewDir) {
}

//------------------------------------------------------------------------
// This function goes through the lines of the bounding box and uses the 
// vertices of each line to draw the bounding box for your volume 
//------------------------------------------------------------------------
void drawBoundingBox() {
	int l;
	
	glTranslatef(-0.5, -0.5, 0.0);	// translate to look at centre of bbox
	glColor3f(1.0, 1.0, 1.0); 
	glBegin(GL_LINES);
		
	for(l = 0; l < BBOX_LINE_COUNT; l++) {
		XYZ p0 = theBboxLines[l].p1;
		XYZ p1 = theBboxLines[l].p2;
		
		glVertex3f(p0.x, p0.y, p0.z); glVertex3f(p1.x, p1.y, p1.z);
	}
		
	glEnd();
}
//------------------------------------------------------------------------
//   It's the main application function. Note the clean code you can
//   obtain using he GLUT library. No calls to dark windows API
//   functions with many obscure parameters list. =)
//------------------------------------------------------------------------
int main(int argc, char** argv) {
//	theBboxsLines[0] = (Line) {{0, 0, 0}, {0, 0, 1}};
	
	/* Glut's initialization code. Set window's size and type of display.
	Window size is 640x480 resolution as defined by above constants. 
	The windows is positioned at the top leftmost area of the screen. 
	Note all glutInit perfrom some kind of initialization */
	
	// Here we specify the display mode of the window: rgba, double buffering, depth buffering
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	// Where the window is first drawn, i.e. top left
	glutInitWindowPosition( GL_WIN_INITIAL_X, GL_WIN_INITIAL_Y );
	// Initial size of our window, i.e. 640x480 resolution
	glutInitWindowSize( GL_WIN_WIDTH, GL_WIN_HEIGHT );
	// Initialize glut itself with pointers to parameters from the main function
	glutInit( &argc, argv );
	// Create a window with given title
	glutCreateWindow("Volume Viewer");
	
	/* The function below are called when the respective event is triggered. */
	glutReshapeFunc(glutResize);       // called every time the screen is resized
	glutDisplayFunc(glutDisplay);      // called when window needs to be redisplayed
	glutMouseFunc(glutMouse);          // called when the application receives a input from the mouse
	glutMotionFunc(glutMotion);        // called when the mouse moves over the screen with one of this button pressed
	
	/* Do any lighting, material, alpha, textures, etc initialization or
	configuration here. */
	InitializeOGL();
	
	/* Application's main loop. all the above functions 
	are called when the respective events care triggered */
	glutMainLoop();
	
	return 0;
}

