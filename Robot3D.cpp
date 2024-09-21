/*******************************************************************
		   Hierarchical Multi-Part Model
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "QuadMesh.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

const int vWidth = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels
int xCenter = vWidth / 2;
int yCenter = vHeight / 2;

using namespace std;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat robotBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat robotBody_mat_specular[] = { 0.5f,0.55f,0.45f,1.0f };
GLfloat robotBody_mat_diffuse[] = { 0.5f,0.2f,0.2f,1.0f };
GLfloat robotBody_mat_shininess[] = { 32.0F };


GLfloat robotArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotArm_mat_diffuse[] = { 0.25f,0.25f,0.25f,1.0f };
GLfloat robotArm_mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat robotArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

GLfloat robotLowerBody_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat robotLowerBody_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat robotLowerBody_mat_shininess[] = { 76.8F };


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

// A flat open mesh
QuadMesh* groundMesh = NULL;


// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
	VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 300;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void gunAnimationHandler(int param);
void walkAnimationHandler(int time);
void drawRobot();
void drawBody();
void drawGun();
void drawUpperLeftArm();
void drawLowerLeftArm();
void drawUpperRightArm();
void drawLowerRightArm();

GLfloat textureMap[64][64][3];
GLuint tex[2];


void assignColor(GLfloat col[3], GLfloat r, GLfloat g, GLfloat b) {
	col[0] = r;
	col[1] = g;
	col[2] = b;
}

int isEven(int x) {
	if (x % 2 == 0)
		return 1;
	else
		return 0;
}

void makeTextureMap()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++) {
			if (isEven(i / 8 + j / 8))
				assignColor(textureMap[i][j], 1.0, 1.0, 1.0);
			else
				assignColor(textureMap[i][j], 1.0, 0.0, 0.0);
		}
}

void makeTextures()
{
	GLfloat planes[] = { 0.0, 0.0, 0.3, 0.0 };
	GLfloat planet[] = { 0.0, 0.3, 0.0, 0.0 };

	glGenTextures(2, tex);

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap);

	/************************************ */
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap);
}

//gluobjects
void drawSphere(float radius, float slices, float stacks) {
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	GLUquadricObj* sphere = gluNewQuadric();

	gluQuadricDrawStyle(sphere, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluSphere(sphere, radius, slices, stacks);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

}

void drawCylinder(float length, float front_radius, float back_radius, float slices, float stacks)
{
	GLUquadricObj* cylinder = gluNewQuadric();

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	gluQuadricTexture(cylinder, GL_TRUE);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluCylinder(cylinder, back_radius, front_radius, length, slices, stacks);
}
class Projectile {

public:
	string id = "player";
	int xAim = 0;
	int yAim = 0;
	GLfloat x = 0;
	GLfloat y = 0;
	GLfloat z = 20;

	Projectile() {}
	Projectile(GLfloat x, GLfloat y, GLfloat z, int xAim, int yAim, string id) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->xAim = xAim;
		this->yAim = yAim;
		this->id = id;
	}

	void drawProj() {

		GLfloat playerRobotBody_color[] = { 1.0f,0.0f,0.0f,1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT, playerRobotBody_color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

		glPushMatrix();
		glTranslatef(x, y, z);

		glScalef(1, 1, 1);
		glutSolidSphere(1, 50, 50);
		glPopMatrix();
	}

	static void moveProj(int value)
	{
		// Get a pointer to the instance of the class
		Projectile* instance = (Projectile*)value;

		// Update the state of the class
		instance->moveProjHandler(10);

		// Set the timer to call the update function again in 1000 ms
		glutTimerFunc(250, moveProj, value);
	}

	void moveProjHandler(int time) {
		if (id == "player") {
			z -= abs(cos(xAim * (3.141592653 / 180)));
		}
		else {
			z += abs(cos(xAim * (3.141592653 / 180)));
		}

		if (xAim >= 0) {
			x -= abs(sin(xAim * (3.141592653 / 180)));
		}
		else {
			x += abs(sin(xAim * (3.141592653 / 180)));
		}
	}
};

// List of all projectiles
std::vector<Projectile> envProjectiles = {};

// Body Rotation
float bodyRotation = 0.0;

// Gun Rotation 
float gunRotation = 0.0;

// Left Leg Rotation
float leftHipRotation = 0.0;
float leftKneeRotation = 0.0;

// Right Leg Rotation
float rightHipRotation = 0.0;
float rightKneeRotation = 0.0;

class Robot {
public:

	// Control Robot Body Rotation
	float robotAngle = 0.0;

	// Robot body scale
	float robotBodyWidth = 8.0;
	float robotBodyLength = 10.0;
	float robotBodyDepth = 6.0;

	// Left Leg Rotation
	float leftHipRotation = 0.0;
	float leftKneeRotation = 0.0;

	// Right Leg Rotation
	float rightHipRotation = 0.0;
	float rightKneeRotation = 0.0;

	int walkforwardDist = 0;

	bool hipState = true;
	bool kneeState = true;
	bool walkState = true;
	bool stop = false;
	bool isAlive = true;
	bool isShot = false;

	GLfloat x = 0;
	GLfloat y = 0;
	GLfloat z = -70;

	// Random Number Range
	int range = 101;

	Robot() {
		x = rand() % range - 50;
	}

	static void death(int value)
	{
		// Get a pointer to the instance of the class
		Robot* instance = (Robot*)value;

		// Update the state of the class
		instance->deathHandler();

		// Set the timer to call the update function again in 1000 ms
		glutTimerFunc(25, death, value);
	}

	void deathHandler() {
		if (isShot && robotAngle <= 120) {
			robotAngle += 3;
		}
		if (robotAngle > 120) {
			isAlive = false;
		}
	}

	static void shoot(int value)
	{
		// Get a pointer to the instance of the class
		Robot* instance = (Robot*)value;

		// Update the state of the class
		instance->shootHandler();

		// Set the timer to call the update function again in 1000 ms
		glutTimerFunc(2000, shoot, value);
	}

	void shootHandler() {
		if (isAlive) {
			Projectile* newProjectile = new Projectile(x, y, z, -180, 0, "bot");
			envProjectiles.push_back(*newProjectile);
		}
	}

	static void walkDistance(int value)
	{
		// Get a pointer to the instance of the class
		Robot* instance = (Robot*)value;

		// Update the state of the class
		instance->walkDistanceHandler(10);

		// Set the timer to call the update function again in 1000 ms
		glutTimerFunc(250, walkDistance, value);
	}

	void walkDistanceHandler(int time) {
		z++;

		if (z > 60) {
			this->isAlive = false;
		}
	}

	void reset() {
		z = -70;
		x = rand() % range - 50;
		this->robotAngle = 0;
		this->isAlive = true;
		this->isShot = false;
	}

	static void walkAnimation(int value)
	{
		// Get a pointer to the instance of the class
		Robot* instance = (Robot*)value;

		// Update the state of the class
		instance->walkAnimationHandler(10);

		// Set the timer to call the update function again in 1000 ms
		glutTimerFunc(10, walkAnimation, value);
	}

	void walkAnimationHandler(int time)
	{
		if (walkState) {
			if (leftHipRotation > 30 or leftHipRotation < -60) {
				hipState = !hipState;
			}

			if (hipState) {
				leftHipRotation -= 2;
				rightHipRotation += 2;
			}
			else {
				leftHipRotation += 2;
				rightHipRotation -= 2;
			}

			if (leftKneeRotation < -30 or leftKneeRotation > 60) {
				kneeState = !kneeState;
			}

			if (kneeState) {
				leftKneeRotation += 2;
				rightKneeRotation -= 2;

			}
			else {
				leftKneeRotation -= 2;
				rightKneeRotation += 2;
			}
			glutPostRedisplay();
		}
	}

	void drawRobot()
	{
		glPushMatrix();

		// Translate robot to the back and shift by random
		glTranslatef(x, y, z);

		// spin robot on base. 
		glRotatef(robotAngle, 1, 0, 0.0);

		// Circle Body
		drawBody();

		// Left Leg
		drawUpperLeftArm();

		// Right Leg
		drawUpperRightArm();

		glPopMatrix();
	}

	void drawBody()
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);
		//GLuint textureID = SOIL_load_OGL_texture("texture.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		// Rotate
		glPushMatrix();
		glRotatef(bodyRotation, -1, 0, 0);

		glPushMatrix();
		glScalef(4, 4, 4);
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//glutSolidSphere(1, 50, 50);
		drawSphere(1, 50, 50);
		glPopMatrix();

		drawGun();

		glPopMatrix();
	}

	void drawGun()
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

		GLUquadricObj* quadratic;
		quadratic = gluNewQuadric();

		glPushMatrix();
		glTranslatef(0, 0, 3);

		// Septagon Gun Base
		gluCylinder(quadratic, 1.5, 1.5, 1.5, 7, 7);

		// Septagon Gun Base covering
		glPushMatrix();
		glTranslatef(0, 0, 1.5);
		gluDisk(quadratic, 0, 1.5, 7, 7);

		// Gun Spining
		glPushMatrix();
		glRotatef(gunRotation, 0, 0, 1);

		// Gun
		gluCylinder(quadratic, 1, 1, 1.5, 7, 7);

		// Gun covering
		glPushMatrix();
		glTranslatef(0, 0, 1.5);
		gluDisk(quadratic, 0, 1, 7, 7);
		glPopMatrix();

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}

	void drawUpperLeftArm()
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

		GLUquadricObj* quadratic;
		quadratic = gluNewQuadric();

		glPushMatrix();
		glTranslatef(0.25 * robotBodyWidth, 0, -2);

		glPushMatrix();
		glRotatef(90, 0, 1, 0);

		//Hip Small Cylinder
		gluCylinder(quadratic, 1.5, 1.5, 3.5, 50, 50);

		glPopMatrix();
		glPopMatrix();

		//----------------------------------------------------

		glPushMatrix();
		glTranslatef(0.25 * robotBodyWidth + 3.5, 0, -2);

		// Hip Rotation
		glPushMatrix();
		glRotatef(leftHipRotation, 1, 0, 0);

		glPushMatrix();
		glRotatef(90, 0, 1, 0);

		glPushMatrix();
		glScalef(1.5, 1.5, 1.5);

		//----------------------------------------------------

		//Hip Big Cylinder + Disks
		gluCylinder(quadratic, 1.5, 1.5, 1, 50, 50);
		gluDisk(quadratic, 0, 1.5, 50, 50);

		glPushMatrix();
		glTranslatef(0, 0, 1);
		gluDisk(quadratic, 0, 1.5, 50, 50);
		glPopMatrix();

		//----------------------------------------------------
		glPushMatrix();
		glTranslated(0, -.5, 0.5);

		glPushMatrix();
		glRotatef(90, 1, 0, 0);

		glPushMatrix();
		glRotatef(20, 0, 1, 0);

		//Upper Leg - Quads
		gluCylinder(quadratic, 0.5, 0.5, 3, 50, 50);

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();


		glPopMatrix();
		glPopMatrix();

		drawLowerLeftArm();

		glPopMatrix();
		glPopMatrix();
	}

	void drawLowerLeftArm()
	{
		GLUquadricObj* quadratic;
		quadratic = gluNewQuadric();

		glPushMatrix();
		glRotatef(90, 0, 1, 0);

		glPushMatrix();
		glScalef(1.5, 1.5, 1.5);

		glPushMatrix();
		glTranslatef(1.25, -5 + 0.5 + 0.5, 0);

		//Lower Leg - Knee + Disks
		gluCylinder(quadratic, 1, 1, 1, 50, 50);
		gluDisk(quadratic, 0, 1, 50, 50);

		glPushMatrix();
		glTranslatef(0, 0, 1);
		gluDisk(quadratic, 0, 1, 50, 50);
		glPopMatrix();

		glPopMatrix();

		//----------------------------------------------------

		glPushMatrix();
		glTranslated(1, -4, 0.5);

		//Knee Rotation
		glPushMatrix();
		glRotatef(leftKneeRotation, 0, 0, 1);


		glPushMatrix();
		glRotatef(90, 1, 0, 0);

		glPushMatrix();
		glRotatef(20, 0, -3, 0);

		//Lower Leg - Shins
		gluCylinder(quadratic, 0.5, 0.5, 4, 50, 50);

		//Lower Leg - Shin Guards 1
		glPushMatrix();
		glTranslatef(0, 0.75, 0);
		glPushMatrix();
		glScalef(1, 0.25, 6);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();

		//Lower Leg - Shin Guards 2
		glPushMatrix();
		glTranslatef(0, -0.75, 0);
		glPushMatrix();
		glScalef(1, 0.25, 6);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();

		//Lower Leg - Foot
		glPushMatrix();
		glTranslatef(-1, 0, 4.25);

		glPushMatrix();
		glScalef(3, 1.5, 1);

		glPushMatrix();
		glRotatef(35, 0, 1, 0);

		glutSolidCube(1);

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

	}

	void drawUpperRightArm()
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

		GLUquadricObj* quadratic;
		quadratic = gluNewQuadric();

		glPushMatrix();
		glTranslatef(-((0.25 * robotBodyWidth) + 3.5), 0, -2);

		glPushMatrix();
		glRotatef(90, 0, 3, 0);

		//Hip Small Cylinder
		gluCylinder(quadratic, 1.5, 1.5, 3.5, 50, 50);

		glPopMatrix();
		glPopMatrix();

		//----------------------------------------------------

		glPushMatrix();
		glTranslatef(-(0.25 * robotBodyWidth + 3.5 + 1.5), 0, -2);

		// Hip Rotation
		glPushMatrix();
		glRotatef(rightHipRotation, 1, 0, 0);

		glPushMatrix();
		glRotatef(90, 0, 3, 0);

		glPushMatrix();
		glScalef(1.5, 1.5, 1.5);

		//----------------------------------------------------

		//Hip Big Cylinder + Disks
		gluCylinder(quadratic, 1.5, 1.5, 1, 50, 50);
		gluDisk(quadratic, 0, 1.5, 50, 50);

		glPushMatrix();
		glTranslatef(0, 0, 1);
		gluDisk(quadratic, 0, 1.5, 50, 50);
		glPopMatrix();


		//----------------------------------------------------
		glPushMatrix();
		glTranslated(0, -.5, 0.5);

		glPushMatrix();
		glRotatef(90, 1, 0, 0);

		glPushMatrix();
		glRotatef(20, 0, 1, 0);

		//Leg - Quads
		gluCylinder(quadratic, 0.5, 0.5, 3, 50, 50);

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPopMatrix();
		glPopMatrix();

		drawLowerRightArm();

		glPopMatrix();
		glPopMatrix();
	}

	void drawLowerRightArm()
	{
		GLUquadricObj* quadratic;
		quadratic = gluNewQuadric();

		glPushMatrix();
		glRotatef(90, 0, 3, 0);

		glPushMatrix();
		glScalef(1.5, 1.5, 1.5);

		//----------------------------------------------------

		glPushMatrix();
		glTranslatef(1.25, -5 + 0.5 + 0.5, 0);


		//Leg - Knee + Disks
		gluCylinder(quadratic, 1, 1, 1, 50, 50);
		gluDisk(quadratic, 0, 1, 50, 50);

		glPushMatrix();
		glTranslatef(0, 0, 1);
		gluDisk(quadratic, 0, 1, 50, 50);
		glPopMatrix();
		glPopMatrix();

		//----------------------------------------------------

		glPushMatrix();
		glTranslated(1, -4, 0.5);

		//Knee Rotation
		glPushMatrix();
		glRotatef(rightKneeRotation, 0, 0, 1);

		glPushMatrix();
		glRotatef(90, 1, 0, 0);

		glPushMatrix();
		glRotatef(20, 0, -1, 0);

		//Leg - Shins
		gluCylinder(quadratic, 0.5, 0.5, 4, 50, 50);

		//Leg - Shin Guards 1
		glPushMatrix();
		glTranslatef(0, 0.75, 0);
		glPushMatrix();
		glScalef(1, 0.25, 6);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();

		//Leg - Shin Guards 2
		glPushMatrix();
		glTranslatef(0, -0.75, 0);
		glPushMatrix();
		glScalef(1, 0.25, 6);
		glutSolidCube(1);
		glPopMatrix();
		glPopMatrix();

		//Leg - Foot
		glPushMatrix();
		glTranslatef(-1, 0, 4.25);

		glPushMatrix();
		glScalef(3, 1.5, 1);

		glPushMatrix();
		glRotatef(35, 0, 1, 0);

		glutSolidCube(1);

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}

};

class Player : public Robot {
public:
	int xAim = 0;
	int yAim = 0;
	int x = 0;
	int y = 0;
	int z = 23;
	bool isShot = false;

	Player() {
	}

	void shootHandler() {
		Projectile* newProjectile = new Projectile(0, 0, 23, this->xAim, this->yAim, "player");
		envProjectiles.push_back(*newProjectile);
	}

	static void shot(int value)
	{
		// Get a pointer to the instance of the class
		Player* instance = (Player*)value;

		// Update the state of the class
		instance->toggleShotHandler();

		// Set the timer to call the update function again in 1000 ms
		glutTimerFunc(1000, shot, value);
	}

	void toggleShotHandler() {
		isShot = false;
	}

	void printAim() {
		cout << xAim << " " << yAim << "\n";
	}
	void drawPlayerRobot()
	{
		glPushMatrix();

		// Translate robot to the back and shift by random
		glTranslatef(x, y, z);

		// spin robot on base. 
		glRotatef(180, 0, -1, 0);

		// Circle Body
		drawBody();

		// Left Leg
		drawUpperLeftArm();

		// Right Leg
		drawUpperRightArm();

		glPopMatrix();

	}

	void drawBody()
	{
		if (isShot) {
			GLfloat playerRobotBody_color[] = { 0.0f,0.0f,1.0f,1.0f };
			glMaterialfv(GL_FRONT, GL_AMBIENT, playerRobotBody_color);
		}
		else {
			GLfloat playerRobotBody_color[] = { 0.0f,1.0f,1.0f,1.0f };
			glMaterialfv(GL_FRONT, GL_AMBIENT, playerRobotBody_color);
		}
		/*GLfloat playerRobotBody_color[] = { 0.0f,0.0f,0.0f,1.0f };*/
		//glMaterialfv(GL_FRONT, GL_AMBIENT, playerRobotBody_color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

		// Rotate
		glPushMatrix();
		glRotatef(bodyRotation, -1, 0, 0);

		glPushMatrix();
		glScalef(4, 4, 4);
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//glutSolidSphere(1, 50, 50);
		drawSphere(1, 50, 50);
		glPopMatrix();

		glRotatef(xAim, 0, 1, 0);
		glRotatef(yAim, 1, 0, 0);
		drawGun();

		glPopMatrix();
	}
};

Robot* robot1 = NULL;
Robot* robot2 = NULL;
Player* playerRobot = NULL;


int main(int argc, char** argv)
{
	srand(time(NULL));
	//texture = LoadTexture("texture.jpg");

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}

// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	robot1 = new Robot();
	glutTimerFunc(10, robot1->walkAnimation, (int)robot1);
	glutTimerFunc(250, robot1->walkDistance, (int)robot1);
	glutTimerFunc(2000, robot1->shoot, (int)robot1);
	glutTimerFunc(25, robot1->death, (int)robot1);

	robot2 = new Robot();
	glutTimerFunc(10, robot2->walkAnimation, (int)robot2);
	glutTimerFunc(250, robot2->walkDistance, (int)robot2);
	glutTimerFunc(2000, robot2->shoot, (int)robot2);
	glutTimerFunc(25, robot2->death, (int)robot2);

	playerRobot = new Player();
	glutTimerFunc(1000, playerRobot->shot, (int)playerRobot);

	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-300.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 64);
	groundMesh->InitMesh(meshSize, origin, 600, 100, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 8.0, 24, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw Robot
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	makeTextureMap();
	makeTextures();
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	// Apply modelling transformations M to move robot
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV

	if (robot1->isAlive) {
		robot1->drawRobot();
	}

	if (robot2->isAlive) {
		robot2->drawRobot();
	}

	playerRobot->drawPlayerRobot();
	playerRobot->printAim();

	if (!robot1->isAlive && !robot2->isAlive) {
		robot1->reset();
		robot2->reset();
	}

	for (int i = 0; i < envProjectiles.size(); ++i) {
		envProjectiles[i].moveProjHandler(10);
		envProjectiles[i].drawProj();

		// Robot 1 Hit 
		if (robot1->x - 8 <= envProjectiles[i].x && robot1->x + 8 >= envProjectiles[i].x) {
			if (robot1->y - 8 <= envProjectiles[i].y && robot1->y + 8 >= envProjectiles[i].y) {
				if (robot1->z - 5 <= envProjectiles[i].z && robot1->z + 5 >= envProjectiles[i].z) {
					if (envProjectiles[i].id != "bot") {
						robot1->isShot = true;
					}
				}
			}
		}

		// Robot 2 Hit 
		if (robot2->x - 8 <= envProjectiles[i].x && robot2->x + 8 >= envProjectiles[i].x) {
			if (robot2->y - 8 <= envProjectiles[i].y && robot2->y + 8 >= envProjectiles[i].y) {
				if (robot2->z - 5 <= envProjectiles[i].z && robot2->z + 5 >= envProjectiles[i].z) {
					if (envProjectiles[i].id != "bot") {
						robot2->isShot = true;
					}
				}
			}
		}

		// Player Hit 
		if (playerRobot->x - 10 <= envProjectiles[i].x && playerRobot->x + 10 >= envProjectiles[i].x) {
			if (playerRobot->y - 10 <= envProjectiles[i].y && playerRobot->y + 10 >= envProjectiles[i].y) {
				if (playerRobot->z - 5 <= envProjectiles[i].z && playerRobot->z + 5 >= envProjectiles[i].z) {
					if (envProjectiles[i].id != "player") {
						playerRobot->isShot = true;
						cout << "Player Hit \n";
					}
				}
			}
		}
	}

	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -15.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}



// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (GLdouble)w / h, 0.2, 300);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	xCenter = w / 2;
	yCenter = h / 2;
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool hipState = true;
bool kneeState = true;
bool walkState = true;
bool stop = false;


// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		/*case 'h':
			leftHipRotation += 2.0;
			break;

		case 'H':
			leftHipRotation -= 2.0;
			break;

		case 'k':
			leftKneeRotation += 2.0;
			break;

		case 'K':
			leftKneeRotation -= 2.0;
			break;

		case 'b':
			bodyRotation += 2.0;
			break;
		case 'B':
			bodyRotation -= 2.0;
			break;

		case 'r':
			robotAngle += 2.0;
			break;
		case 'R':
			robotAngle -= 2.0;
			break;

		case 'c':
			stop = false;
			glutTimerFunc(10, gunAnimationHandler, 0);
			break;
		case 'C':
			stop = true;
			break;

		case 'w':
			walkState = true;
			glutTimerFunc(10, walkAnimationHandler, 0);
			break;


		case 'W':
			walkState = false;
			leftHipRotation = 0;
			leftKneeRotation = 0;
			rightHipRotation = 0;
			rightKneeRotation = 0;
			break;*/

	case 'w':
		playerRobot->yAim += 3;
		break;

	case 's':
		playerRobot->yAim -= 3;
		break;

	case 'a':
		playerRobot->xAim += 3;
		break;

	case 'd':
		playerRobot->xAim -= 3;
		break;

	case ' ':
		playerRobot->shootHandler();
		break;
	}


	glutPostRedisplay();   // Trigger a window redisplay
}

void gunAnimationHandler(int param)
{
	if (!stop)
	{
		gunRotation += 1.0;
		glutPostRedisplay();
		glutTimerFunc(10, gunAnimationHandler, 0);
	}
}

void walkAnimationHandler(int time)
{
	if (walkState) {
		if (leftHipRotation > 30 or leftHipRotation < -60) {
			hipState = !hipState;
		}

		if (hipState) {
			leftHipRotation -= 2;
			rightHipRotation += 2;
		}
		else {
			leftHipRotation += 2;
			rightHipRotation -= 2;
		}

		if (leftKneeRotation < -30 or leftKneeRotation > 60) {
			kneeState = !kneeState;
		}

		if (kneeState) {
			leftKneeRotation += 2;
			rightKneeRotation -= 2;

		}
		else {
			leftKneeRotation -= 2;
			rightKneeRotation += 2;
		}

		glutPostRedisplay();
		glutTimerFunc(10, walkAnimationHandler, 0);
	}
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{

	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	// glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	// glutPostRedisplay();   // Trigger a window redisplay
}


float sens = 0.15;

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	int xOffset = xMouse - xCenter;
	int yOffset = yMouse - yCenter;
	GLfloat xAngle = -cos(xOffset / xCenter) * xOffset * sens;
	GLfloat yAngle = cos(yOffset / yCenter) * yOffset * sens;

	cout << "Mouse: " << xAngle << " " << yAngle << "\n";
	playerRobot->xAim = xAngle;
	playerRobot->yAim = yAngle;

	glutPostRedisplay();   // Trigger a window redisplay
}

