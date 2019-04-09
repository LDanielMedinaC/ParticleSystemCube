#include "Particle.h"

Particle::Particle() {
	
	position = new float[3];
	oldPos = new float[3];

	position[0] = oldPos[0] = 0;
	position[1] = oldPos[1] = 0;
	position[2] = oldPos[2] = 0;

	radius = 0.1 + (0.1) * (float)rand() / RAND_MAX;
	mass = radius;

	float fx = -1.0f + 2.0f * (float)rand() / RAND_MAX;
	float fy = -1.0f + 2.0f * (float)rand() / RAND_MAX;
	float fz = -1.0f + 2.0f * (float)rand() / RAND_MAX;

	forces = new float[3];
	forces[0] = 10000 * fx;
	forces[1] = 10000 * fy;
	forces[2] = 10000 * fz;

	area = M_PI * radius*radius;
	restitutionCoefficient = 0.5;//1.0f;///set in one if you want a perfectly elastic 

	dragForce = new float[3];
	dragForce[0] = dragForce[1] = dragForce[2] = area / 10.0f;
	
	ka = new GLfloat[4];
	ka[0] = 0.05 + (0.2 - 0.05) * (float)rand() / RAND_MAX;
	ka[1] = 0.05 + (0.2 - 0.05) * (float)rand() / RAND_MAX;
	ka[2] = 0.05 + (0.2 - 0.05) * (float)rand() / RAND_MAX;
	ka[3] = 1.0f;

	kd = new GLfloat[4];
	kd[0] = 0.2 + (0.8 - 0.2) * (float)rand() / RAND_MAX;
	kd[1] = 0.2 + (1.0 - 0.2) * (float)rand() / RAND_MAX;
	kd[2] = 0.2 + (1.0 - 0.2) * (float)rand() / RAND_MAX;
	kd[3] = 1.0f;

	kd2 = new GLfloat[4];
	kd2[0] = 1.0;
	kd2[1] = 0.0f;
	kd2[2] = 0.0f;
	kd2[3] = 1.0f;

	ks = new GLfloat[4];
	ks[0] = 0.4 + (0.8 - 0.4) * (float)rand() / RAND_MAX;
	ks[1] = 0.4 + (0.8 - 0.4) * (float)rand() / RAND_MAX;
	ks[2] = 0.4 + (0.8 - 0.4) * (float)rand() / RAND_MAX;
	ks[3] = 1.0f;

	alpha = new GLfloat[1];
	alpha[0] = 100;

	isColliding = false;
	dt = 0.01f;
}

Particle::~Particle() {}

void Particle::draw() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
	if (isColliding) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd2);
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, alpha);
	glPushMatrix(); {
		glTranslatef(position[0], position[1], position[2]);
		glutSolidSphere(radius, 20, 20);
	} glPopMatrix();
}

void Particle::integrateVerlet(float dt) {
	float temp[] = {position[0], position[1], position[2]};

	float accel[3];

	accel[0] = (dragForce[0] + forces[0]) / mass;
	accel[1] = (dragForce[1] + forces[1]) / mass;
	accel[2] = (dragForce[2] + forces[2]) / mass;

	position[0] = (2 * position[0]) - oldPos[0] + accel[0] * dt * dt;
	position[1] = (2 * position[1]) - oldPos[1] + accel[1] * dt * dt;
	position[2] = (2 * position[2]) - oldPos[2] + accel[2] * dt * dt;

	oldPos[0] = temp[0];
	oldPos[1] = temp[1];
	oldPos[2] = temp[2];

}

void Particle::checkWallCollision() {

	for (int i = 0; i < 3; i++) {
		if (position[i] <= radius - 10.0f) {
			position[i] = radius - 10 + radius / 100.f;
			forces[i] = -forces[i] * restitutionCoefficient;
		}
		if (position[i] >= radius + 10.0f) {
			position[i] = radius + 10 - radius / 100.f;
			forces[i] = -forces[i] * restitutionCoefficient;
		}
		if (dragForce[i] * forces[i] < 0)
			dragForce[i] = -dragForce[i];
		if (dragForce[i] * dragForce[i] > forces[i] * forces[i])
			dragForce[i] *= 0.5;
		if (forces[i] * forces[i] < 0.0000001f) {
			forces[i] = 0;
			dragForce[i] = 0;
		}
	}

}

void Particle::update() {
	forces[1] += (-9.81*mass/60.0f);
	///Stop particles with drag forces, it doesnt work with y, IDK why
	/*for (int i = 0; i < 3; i++) {
		//printf("%f", forces[i]);
		forces[i] -= dragForce[i];
		//printf("------ %f\n", forces[i]);
		dragForce[i] *= 0.9999999f;
		
	}*/
	integrateVerlet(dt);
	checkWallCollision();
}

bool Particle::inCollision(Particle * p) {

	float sumR = (radius + p->radius)*(radius + p->radius);
	

	float distance2 = (position[0] - p->position[0])*(position[0] - p->position[0]) + (position[1] - p->position[1])*(position[1] - p->position[1]) + (position[2] - p->position[2])*(position[2] - p->position[2]);

	if (sumR > distance2) {


		float temp[] = {forces[0],forces[1], forces[2]};
		for (int i = 0; i < 3; i++) {
			float va = position[i] - oldPos[i];
			float vb = p->position[i] - p->oldPos[i];
			if (va*vb < 0) {
				forces[i] = -forces[i];
				p->forces[i] = -p->forces[i];
				dragForce[i] = -dragForce[i];
				p->dragForce[i] = -p->dragForce[i];
				/*if (va < 0) {
					position[i] -= 0.000001f;
					p -> position[i] += 0.00001f;
				}
				else {
					position[i] += 0.00001f;
					p->position[i] -= 0.00001f;
				}*/
			}
		}
		return true;
	}
	return false;
}

