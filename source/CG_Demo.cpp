/*
 * Sergio Ruiz.
 *
 * TC3022. Computer Graphics Course.
 * Basic template OpenGL project.
 */

// Please don't change lines 9-31 (It helps me to grade)
#ifdef __APPLE__
// For XCode only: New C++ terminal project. Build phases->Compile with libraries: add OpenGL and GLUT
// Import this whole code into a new C++ file (main.cpp, for example). Then run.
// Reference: https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/Mac
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
#endif
#ifdef _WIN32
// For VS on Windows only: Download CG_Demo.zip. UNZIP FIRST. Double click on CG_Demo/CG_Demo.sln
// Run
	#include "freeglut.h"
#endif
#ifdef __unix__
// For Linux users only: g++ CG_Demo.cpp -lglut -lGL -o CG_Demo
// ./CG_Demo
// Reference: https://www.linuxjournal.com/content/introduction-opengl-programming
	#include "GL/freeglut.h"
	#include "GL/gl.h"
#endif

#include <stdio.h>
#include <math.h>

#include "Particle.h"

GLfloat *Ia, *Is, *Id, *Ip;

Particle ** particles;
float dt;

#define NUM_PARTICLES 100

void rectangle(float x, float w, float y, float h, float r, float g, float b)
{
	glPushMatrix(); {
		glColor3f(r, g, b);
		glBegin(GL_POLYGON);
		{
			glVertex3f(x, y, 0);
			glVertex3f(x + w, y, 0);
			glVertex3f(x + w, y - h, 0);
			glVertex3f(x, y - h, 0);
		}
		glEnd;
	}
	glPopMatrix();
}

void init() // FOR GLUT LOOP
{

	Ia = new GLfloat[4]; // Ambient
	Id = new GLfloat[4]; // Diffuse
	Is = new GLfloat[4]; // Specular
	Ip = new GLfloat[4]; // Position


	Ia[0] = 1.0f;
	Ia[1] = 1.0f;
	Ia[2] = 1.0f;
	Ia[3] = 1.0f;

	Id[0] = 1.0f;
	Id[1] = 1.0f;
	Id[2] = 1.0f;
	Id[3] = 1.0f;

	Is[0] = 1.0f;
	Is[1] = 1.0f;
	Is[2] = 1.0f;
	Is[3] = 1.0f;

	Ip[0] = 0.0f;
	Ip[1] = 15.0f;
	Ip[2] = 20.0f;
	Ip[3] = 0.0f;	// If value is 1.0f it's a point light
					// If value is 0.0f it's a directional light

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, Ia);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Id);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Is);
	glLightfv(GL_LIGHT0, GL_POSITION, Ip);

	dt = 0.02f;

	particles = new Particle * [NUM_PARTICLES];
	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i] = new Particle(0.1, 0.2);
	}

	glEnable(GL_DEPTH_TEST);			// Enable check for close and far objects.
	glClearColor(0.0, 0.0, 0.0, 0.0);	// Clear the color state.
	glMatrixMode(GL_MODELVIEW);			// Go to 3D mode.
	glLoadIdentity();					// Reset 3D view matrix.
}

void display()							// Called for each frame (about 60 times per second).
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear color and depth buffers.
	glLoadIdentity();												// Reset 3D view matrix.
	gluLookAt(0.0, 0, 40.0,										// Where the camera is.
		      0.0, 0.0, 0.0,										// To where the camera points at.
		      0.0, 1.0, 0.0);										// "UP" vector.
	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i]->draw();
	}
	glutSwapBuffers();												// Swap the hidden and visible buffers.
}

void idle()															// Called when drawing is finished.
{
	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i]->update();
		bool crashed = false;
		for (int j = 0; j < NUM_PARTICLES; j++) {
			if (i != j) {
				if (particles[i]->inCollision(particles[j])) {
					crashed = true;
					particles[i]->isColliding = true;
				}
			}
		}
		if (!crashed) {
			particles[i]->isColliding = false;
		}
	}

	//printf("%.3f, %.3f, %.3f\n", p->forces[0], p->forces[1], p->forces[2]);

	glutPostRedisplay();											// Display again.
}

void reshape(int x, int y)											// Called when the window geometry changes.
{
	glMatrixMode(GL_PROJECTION);									// Go to 2D mode.
	glLoadIdentity();												// Reset the 2D matrix.
	gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 100.0);		// Configure the camera lens aperture.
	glMatrixMode(GL_MODELVIEW);										// Go to 3D mode.
	glViewport(0, 0, x, y);											// Configure the camera frame dimensions.
	gluLookAt(0.0, 1.0, 4.0,
		      0.0, 0.0, 0.0,
		      0.0, 1.0, 0.0);
	display();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);											// Init GLUT with command line parameters.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);		// Use 2 buffers (hidden and visible). Use the depth buffer. Use 3 color channels.
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG first program");
	
	init();
	glutReshapeFunc(reshape);										// Reshape CALLBACK function.
	glutDisplayFunc(display);										// Display CALLBACK function.
	glutIdleFunc(idle);												// Idle CALLBACK function.
	glutMainLoop();													// Begin graphics program.
	return 0;														// ANSI C requires a return value.
}
