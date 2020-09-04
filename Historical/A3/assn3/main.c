/*
Base code for COMP-557 Assignment #1 
(adapted from basecode on www.nehe.gamedev.com)
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
#define FOV    30
#define EPSILON .5
#define DELTA_d  .5
#define OPACITY  200

/* creates a enum type for mouse buttons */
enum {
    BUTTON_LEFT = 0,
    BUTTON_RIGHT,
    BUTTON_LEFT_TRANSLATE,
};

/* set global variables */
int mButton = -1;
int mOldY, mOldX;
int fullscreen = 0;
int frame_count = 1;

/* XYZs that for translation & rotation of the camera */

XYZ lookat = {0.0f, 0.0f, 0.0f};   // MIKE:   look at center of box.
XYZ eye =    {0.0f, 0.0f, 5.0f};   //

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

// width, height, depth of texture
#define WIDTH   4
#define HEIGHT  4
#define DEPTH   4
#define BYTES_PER_TEXEL 4
#define LAYER(r) (WIDTH * HEIGHT * r * BYTES_PER_TEXEL)
// 2->1 dimension mapping function
#define TEXEL2(s, t) (BYTES_PER_TEXEL * (s * WIDTH + t))
// 3->1 dimension mapping function
#define TEXEL3(s, t, r)	(TEXEL2(s, t) + LAYER(r))
	
/* function prototypes */
void glutResize( int w, int h);
void clamp(XYZ v);
void glutMotion(int x, int y);
void glutMouse(int button, int state, int x, int y);
void glutDisplay(void);
void InitializeOGL();
void setUpTextures(char* fileName);
void drawVolume(XYZ viewDir);
unsigned char* makeTextureFromPPM(const char* filename);
unsigned char* buildTexture();
void drawBoundingBox();
int WriteTGAFile( char *filename, short int width, short int height, unsigned char* imageData );
void SaveScreenshot(int width, int height); 

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
    	/* rotates screen */
	rot.x -= (mOldY - y);
	rot.y -= (mOldX - x);
	clamp (rot);
  }
  else if (mButton == BUTTON_RIGHT) {
    /*
      translate the screen, z axis
      gives the idea of zooming in and out 
    */        
    eye.z += (mOldY - y) * 0.01f; 

    //  MIKE:   This is not working so well.  Not sure why.....
    //
    // here I multiply by a 0.05 factor to 
    // slow down the zoom
    //  MIKE:  comment this out    clamp (rot);
  } 
  else if (mButton == BUTTON_LEFT_TRANSLATE) {
    eye.x += (mOldX - x) * 0.01f;
    eye.y -= (mOldY - y) * 0.01f;


    //  MIKE:   NEEDS TO BE DROPPED
    //    clamp (rot);
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
      if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
	mButton = BUTTON_LEFT_TRANSLATE;
	break;
      } 
      else {
	mButton = BUTTON_LEFT;
	break;
      }
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
	
	// Rotate the screen by the angles provided by rot XYZ
	glRotatef(rot.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rot.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
	
	// compute viewing direction
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
	SaveScreenshot(GL_WIN_WIDTH, GL_WIN_HEIGHT);
	
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

	//  MIKE:  Why are we enabling environment mapping below?
	//   I should look up what these mysterious instructions are
	//  doing... 

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
         // our texture colors will replace the untextured colors

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	//   MIKE:  make one vertex have opacity 1

        texels[TEXEL3(WIDTH/2, HEIGHT/2, 1)+3] = 255;
         

	// request texture id from OpenGL
	glGenTextures(1, &texID);	
	// tell OpenGL we're going to be setting up the texture id it gave us	
	glBindTexture(GL_TEXTURE_3D, texID);	
	// when this texture needs to be shrunk to fit on small
	// polygons, use linear interpolation of the texels to determine the color
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// when this texture needs to be magnified to fit on a big
	// polygon, use linear interpolation of the texels to determine the color
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// we want the texture to repeat over the S axis, so if we
	// specify coordinates out of range we still get textured.
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// same as above for T axis
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// same as above for R axis
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	// this is a 3d texture, level 0 (max detail), GL should store
	// it in RGB8 format, 
	// its WIDTHxHEIGHTxDEPTH in size, 
	// it doesnt have a border, we're giving it to GL in RGB
	// format as a series 
	// of unsigned bytes, and texels is where the texel data is.

	//  MIKE:   Marta's code had GL_RGB8 for third parameter,  rather than GL_RGBA8  (Kind of annoying....)

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
	return texels;
}

void drawVolume(XYZ viewDir) {

  int    i, j;
  int    polyct;
  double  min = 1000;     //  initialization for min and max depth  of cube
  double  max = -1000;
  double  tmp, num, den, t, d;
  XYZ     centerXYZ, point1, point2, p2minusp1, tmpXYZ, v, up, right;

 struct  {
   XYZ    xyz;
   double theta;
   double  r;
   double  u;
 } tmppoly, poly[6];     //  We have a maximum of 6 vertices in our
			 //  polygon slice through the cube.

  //  Find minimum and maximum distances from eye to vertices. 

  for (i=0; i<8; i++){   // 8 corners in the cube
    tmp = dotProduct( subtract(theBbox[i], eye), viewDir);
    if (tmp < min)
      min = tmp;
    if (tmp > max)
      max = tmp;
  }

  //   Define a set of layers that slice through the cube. 

  //  for (d = max+ EPSILON; d > min; d-= DELTA_d){

  d = (min + max)/2;
  polyct = 0;

  for (i=0; i < BBOX_LINE_COUNT; i++){

    //  For each line, find where the plane (at distance d) intersects 
    //  this line.    The point of intersection is 'v'.

    point1 = theBboxLines[i].p1;
    point2 = theBboxLines[i].p2;

    //    Use the following formulas.   These depend on eye and
    //    viewDir (which may have been changed by the mouse inputs)
    //    but they assume cube coordinates are not changing i.e. not 
    //    translated or rotated. 
    //
    //    <viewDir, ( point1 + t (point2 - point1) - eye) = d
    //
    //    t = ( d + <viewDIr, e - point1> ) / <viewDir, point2 - point1>

    tmpXYZ = subtract(eye, point1);
    num    = d + dotProduct(viewDir, tmpXYZ);  // numerator
    p2minusp1 = subtract(point2, point1);
    den    = dotProduct(viewDir, p2minusp1);      // denominator

    if  (den != 0){  //  view direction cannot be parallel to line segment
      t =  num / den;
      tmpXYZ =  multiply(  t, p2minusp1);   

      //  Compute v,  the point of intersection.   
      //  Test if v is on the line segment and if so then add it to 
      //  array of vertices.

      v = plus(point1, tmpXYZ);           
      if ((t >= 0) && (t <= 1)){   
	poly[polyct].xyz = v;
	polyct++;
      }
    }
  }

  //   -------------- SORT VERTICES -----------------  
  //
  // Find center of polygon (mean of vertices).  This will be the
  // origin for computing theta.

  tmpXYZ = (poly[0]).xyz;   
  for (j=1; j < polyct; j++){
    tmpXYZ  = plus(poly[j].xyz, tmpXYZ);   
  }
  centerXYZ = multiply(1.0 / polyct, tmpXYZ);

  //  Measure the angle of the vertex in the plane, relative to the 
  //  center point and in the usual local coordinates axes:  'up' is 
  //  the projection of VUP vector and 'right' is cross(viewDir, up)
  //  
  //  So, first compute up and right...

  tmp = dotProduct(updir,viewDir); 
  tmpXYZ = multiply(tmp, viewDir);
  tmpXYZ = subtract(updir, tmpXYZ); 
  up  = normalize(tmpXYZ);
  right = crossProduct(viewDir, up); 

  //   ...  and then compute theta for each point

  for (j=0; j< polyct; j++){
    tmpXYZ   = subtract(poly[j].xyz, centerXYZ);
    poly[j].r =  dotProduct(right, tmpXYZ);
    poly[j].u =  dotProduct(up,    tmpXYZ);
    poly[j].theta = atan2( poly[j].r, poly[j].u);
  }

  // Bubblesort on theta.

  for (i=0; i < polyct; i++)
    for (j=0; j < polyct-1; j++)
      if (poly[j].theta > poly[j+1].theta){   // then swap
	tmppoly.theta = poly[j].theta;
	poly[j].theta = poly[j+1].theta;
	poly[j+1].theta = tmppoly.theta;
	tmppoly.xyz = poly[j].xyz;
	poly[j].xyz = poly[j+1].xyz;
	poly[j+1].xyz = tmppoly.xyz;
      }

  // Draw polygon with sorted vertices.   These get passed through the
  // glRotate and glTranslate commands so they stay attached to the
  // bounding volume.

     glEnable(GL_TEXTURE_3D);
     glEnable(GL_BLEND);
     glBegin(GL_POLYGON);
     for (j = 0; j < polyct; j++){
        tmpXYZ = poly[j].xyz;
	glTexCoord3f(tmpXYZ.x , tmpXYZ.y , tmpXYZ.z);
	glVertex3f(tmpXYZ.x , tmpXYZ.y , tmpXYZ.z);
     }
     glEnd();
     glDisable(GL_BLEND);
     glDisable(GL_TEXTURE_3D);


}
//}

void drawBoundingBox() {
	int l;

	glTranslatef(-0.5, -0.5, 0);
	glColor3f(1, 1, 1); 
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

//------------------------------------------------------
// Write the contents of the buffer to given tga file
//------------------------------------------------------
int WriteTGAFile( char *filename, short int width, short int height, unsigned char* imageData ) {
	FILE *fptr;
	int i;
	fptr = fopen(filename, "wb");
	if( !fptr ) {
		fclose(fptr);
		return 0;
	}

	putc(0,fptr);
	putc(0,fptr);
  	putc(2,fptr);                         /* uncompressed RGB */
  	putc(0,fptr); putc(0,fptr);
  	putc(0,fptr); putc(0,fptr);
  	putc(0,fptr);
  	putc(0,fptr); putc(0,fptr);           /* X origin */
  	putc(0,fptr); putc(0,fptr);           /* y origin */
  	putc((width & 0x00FF),fptr);
  	putc((width & 0xFF00) / 256,fptr);
  	putc((height & 0x00FF),fptr);
  	putc((height & 0xFF00) / 256,fptr);
  	putc(24,fptr);                        /* 24 bit bitmap */
  	putc(0,fptr);

	for( i = 0; i < width*height*3; i+=3 ) {
		putc(imageData[i+2],fptr);
		putc(imageData[i+1],fptr);
		putc(imageData[i],fptr);
	}
	fclose(fptr);

	return 1;

}

//-----------------------------------------------------------------------------------------
// Dump the contents of the framebuffer to a tga file
//-----------------------------------------------------------------------------------------



void SaveScreenshot(int width, int height) {
	char tempfn[100];
	char *basename = "out_";
	sprintf(tempfn, "%s%04d.tga", basename, frame_count++);
	unsigned char* imageData;
	imageData = malloc(width*height*3);		// allocate memory for the imageData
	memset(imageData, 0, width*height*3);	// clear imageData memory contents

	// read the image data from the window
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	// write the image data to a file
	WriteTGAFile(tempfn, (short int)width, (short int)height, (unsigned char*)imageData);

	// free the image data memory
	free(imageData);
}

int main(int argc, char** argv) {
	
	/* Glut's initialization code. Set window's size and type of display.
	Window size is put half the 800x600 resolution as defined by above
	constants. The windows is positioned at the top leftmost area of
	the screen. Note all glutInit perfrom some kind of initialization */
	
	// Here we specify the display mode of the window: rgba, double buffering, depth buffering
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	// Where the window is first drawn, i.e. top left
	glutInitWindowPosition( GL_WIN_INITIAL_X, GL_WIN_INITIAL_Y );
	// Initial size of our window, i.e. 800x600 resolution
	glutInitWindowSize( GL_WIN_WIDTH, GL_WIN_HEIGHT );
	// Initic000lize glut itself with pointers to parameters from the main function
	glutInit( &argc, argv );
	// Create a window with given title
	glutCreateWindow("Volume Viewer");
	
	/* The function below are called when the respective event is triggered. */
	glutReshapeFunc(glutResize);       // called every time the screen is resized
	glutDisplayFunc(glutDisplay);      // called when window needs to be redisplc000yed
	glutMouseFunc(glutMouse);          // called when the c000pplicc000tion receives c000 input from the mouse
	glutMotionFunc(glutMotion);        // called when the mouse moves over the screen with one of this button pressed
	
	/* Do any lighting, material, alpha, textures, etc initialization or
	configuration here. */
	InitializeOGL();
	
	/* Application's main loop. all the above functions 
	are called when the respective events care triggered */
	glutMainLoop();
	
	return 0;
}



