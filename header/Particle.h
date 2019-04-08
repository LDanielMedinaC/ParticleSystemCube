#pragma once
#ifdef _APPLE_
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
#ifdef _unix_
// For Linux users only: g++ CG_Demo.cpp -lglut -lGL -o CG_Demo
// ./CG_Demo
// Reference: https://www.linuxjournal.com/content/introduction-opengl-programming
#include "GL/freeglut.h"
#include "GL/gl.h"
#endif
#ifndef _PARTICLE
#define _PARTICLE

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

class Particle {
public:
	float * position;
	float * oldPos;
	float mass;
	float * color;
	float radius;
	float * forces;
	float * dragForce;
	float area;
	float restitutionCoefficient;
	float dt;

	bool isColliding;

	GLfloat *ka, *ks, *kd, *kd2, *alpha;

	Particle(float minR, float maxR);
	~Particle();
	void draw();
	void checkWallCollision();
	void integrateVerlet(float dt);
	void update();
	bool inCollision(Particle * other);
};
#endif // !_PARTICLE