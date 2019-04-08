#include "Particle.h"

Particle::Particle(float minR, float maxR) {
	
	position = new float[3];
	oldPos = new float[3];

	position[0] = oldPos[0] = 0;
	position[1] = oldPos[1] = 0;
	position[2] = oldPos[2] = 0;

	radius = minR + (maxR - minR) * (float)rand() / RAND_MAX;
	mass = radius;

	float fx = -1.0f + 2.0f * (float)rand() / RAND_MAX;
	float fy = -1.0f + 2.0f * (float)rand() / RAND_MAX;
	float fz = -1.0f + 2.0f * (float)rand() / RAND_MAX;

	forces = new float[3];
	forces[0] = 10 * fx;
	forces[1] = 10 * fy;
	forces[2] = 10 * fz;

	area = M_PI * radius*radius;
	restitutionCoefficient = 1.0f;

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

	float * accel = new float[3];

	if ((forces[0] > 0 && dragForce[0] > 0) || 
		(forces[0] < 0 && dragForce[0] < 0)) {
		dragForce[0] *= -1; }
	if ((forces[1] > 0 && dragForce[1] > 0) ||
		(forces[1] < 0 && dragForce[1] < 0)) {
		dragForce[1] *= -1;
	}
	if ((forces[2] > 0 && dragForce[2] > 0) ||
		(forces[2] < 0 && dragForce[2] < 0)) {
		dragForce[2] *= -1;
	}

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
		if (position[i] >= radius + 10) {
			position[i] = radius + 10 - radius / 100.f;
			forces[i] = -forces[i] * restitutionCoefficient;
		}
	}

}

void Particle::update() {
	//forces[1] += (-9.81*mass*dt);
	integrateVerlet(dt);
	checkWallCollision();
}

bool Particle::inCollision(Particle * other) {

	float sumR = radius + other->radius;
	float sumR2 = sumR * sumR;

	float * nrm = new float[3];
	nrm[0] = position[0] - other->position[0];
	nrm[1] = position[1] - other->position[1];
	nrm[2] = position[2] - other->position[2];

	float distance2 = (nrm[0]* nrm[0]) + (nrm[1]* nrm[1]) + (nrm[2]* nrm[2]);

	if (sumR2 > distance2) {

		/*
		float * dir = new float[3];
		dir[0] = position[0] - oldPos[0];
		dir[1] = position[1] - oldPos[1];
		dir[2] = position[2] - oldPos[2];

		float ndotu = nrm[0] * dir[0]
			+ nrm[1] * dir[1]
			+ nrm[2] * dir[2];

		float * up = new float[3];
		up[0] = nrm[0] * ndotu;
		up[1] = nrm[1] * ndotu;
		up[2] = nrm[2] * ndotu;

		float * uo = new float[3];
		uo[0] = dir[0] - up[0];
		uo[1] = dir[1] - up[1];
		uo[2] = dir[2] - up[2];

		float * ur = new float[3];
		ur[0] += up[0] - uo[0];
		ur[1] += up[1] - uo[1];
		ur[2] += up[2] - uo[2];

		float urmag = pow(ur[0] * ur[0] + ur[1] * ur[1] + ur[2] * ur[2],2);
		ur[0] = ur[0] / urmag;
		ur[1] = ur[1] / urmag;
		ur[2] = ur[2] / urmag;

		position[0] = position[0] + nrm[0] / 100;
		position[1] = position[1] + nrm[1] / 100;
		position[2] = position[2] + nrm[2] / 100;

		forces[0] += forces[0] * ur[0] * restitutionCoefficient;
		forces[1] += forces[1] * ur[1] * restitutionCoefficient;
		forces[2] += forces[2] * ur[2] * restitutionCoefficient;

		free(dir);
		free(up);
		free(uo);
		free(ur);
		*/
		float temp[] = {forces[0],forces[1], forces[2]};
		free(nrm);
		return true;
	}
	else free(nrm); return false;
}

