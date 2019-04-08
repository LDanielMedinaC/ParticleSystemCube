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
#include <list> 
#include <iterator> 

#include "Particle.h"


using namespace std;

GLfloat *Ia, *Is, *Id, *Ip;

Particle ** particles;
list<Particle*> lParticles[203];
float dt;

#define NUM_PARTICLES 100


void iniLight() {
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
}


void addList(Particle * p) {
	int x = (p -> position[0] + 10) * 10;
	lParticles[x].push_back(p);
}

void iniParticles() {
	particles = new Particle *[NUM_PARTICLES];
	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i] = new Particle(0.1, 0.2);
		addList(particles[i]);
	}
}

void init() // FOR GLUT LOOP
{
	iniLight();
	iniParticles();

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


void updateParticles() {
	for (int i = 0; i <= 201; i++) {
		while (!lParticles[i].empty()) {
			Particle * p = lParticles[i].front();
			lParticles[i].pop_front();
			p->update();
			addList(p);
		}
	}
}

bool checkCrash(int pos, Particle *p) {
	bool crashed = false;
	for (list <Particle *> ::iterator it = lParticles[pos].begin(); it != lParticles[pos].end(); ++it) {
		if (p == *it)
			continue\;
		if (p->inCollision(*it)) {
			crashed = true;
			p->isColliding = true;
		}
	}
	return crashed;
}

void checkCrashes() {
	for (int i = 0; i <= 201; i++) {
		for (list <Particle *> ::iterator it = lParticles[i].begin(); it != lParticles[i].end(); ++it) {
			bool crashed = false;
			Particle *p = *it;
			for(int j = -2; j < 3; j++)
				if(j >= 0)
					crashed |= checkCrash(j+i, *it);
			if (!crashed)
				p->isColliding = false;
		}
	}
}

void idle()															// Called when drawing is finished.
{
	updateParticles();
	checkCrashes();
	/*checkCrashes();
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
	}*/

	

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
