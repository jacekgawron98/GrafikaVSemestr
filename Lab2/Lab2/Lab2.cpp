#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>
#include <math.h>

using namespace std;

typedef float vector3[3];
namespace axis {
	enum rotationAxis {
		x = 0,
		y = 1,
		z = 2
	};
}

namespace eggType {
	enum eggType {
		triangle,
		wire,
		dot
	};
}

int N;
float height = 0;
vector3** points;
axis::rotationAxis curAxis = axis::x;
eggType::eggType curEggType = eggType::dot;

vector3 rotationTheta = { 0.0f, 0.0f, 0.0f };

void egg() {
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -height / 2, 0.0f);
	if (curEggType == eggType::dot) {
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (int u = 0; u < N; u++) {
			for (int v = 0; v < N; v++) {
				glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
			}
		}
		glEnd();
	}
	else if (curEggType == eggType::wire) {
		glBegin(GL_LINES);
		for (int u = 0; u < N; u++) {
			for (int v = 0; v < N; v++) {
				if (v != N - 1) {
					glVertex3f(points[u][v + 1][0], points[u][v + 1][1], points[u][v + 1][2]);
				}
				else {
					glVertex3f(points[u][0][0], points[u][0][1], points[u][0][2]);
				}
				glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
			}
		}
		glEnd();
		glBegin(GL_LINES);
		for (int u = 0; u < N; u++) {
			for (int v = 0; v < N; v++) {
				if (u != N - 1) {
					glVertex3f(points[u + 1][v][0], points[u + 1][v][1], points[u + 1][v][2]);
				}
				else {
					glVertex3f(points[0][v][0], points[0][v][1], points[0][v][2]);
				}
				glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
			}
		}
		glEnd();
	}
	else if (curEggType == eggType::triangle) {
		for (int u = 0; u < N; u++) {
			for (int v = 0; v < N; v++) {
				int vdiff = 0, udiff = 0;
				if (v == N - 1) {
					vdiff = 1;
				}
				if (u == N - 1) {
					udiff = 1;
				}

				glBegin(GL_TRIANGLES);
				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(points[u][(v + 1 - N * vdiff)][0], 
					points[u][(v + 1 - N * vdiff)][1], 
					points[u][(v + 1 - N * vdiff)][2]);
				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3f(points[(u + 1 - N * udiff)][v][0], 
					points[(u + 1 - N * udiff)][v][1], 
					points[(u + 1 - N * udiff)][v][2]);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(points[u][v][0], 
					points[u][v][1], 
					points[u][v][2]);
				glEnd();

				glBegin(GL_TRIANGLES);
				glColor3f(1.0f, 0.0f, 1.0f);
				glVertex3f(points[u][v + 1 - N * vdiff][0], 
					points[u][(v + 1 - N * vdiff)][1], 
					points[u][(v + 1 - N * vdiff)][2]);

				glColor3f(0.0f, 1.0f, 1.0f);
				glVertex3f(points[(u + 1 - N * udiff)][v][0], 
					points[(u + 1 - N * udiff)][v][1], 
					points[(u + 1 - N * udiff)][v][2]);

				glColor3f(1.0f, 1.0f, 0.0f);
				glVertex3f(points[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][0], 
					points[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][1], 
					points[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][2]);
				glEnd();
			}
		}
	}
}

void rotateEgg() {
	rotationTheta[curAxis] += 0.1f;
	if (rotationTheta[curAxis] > 360) {
		rotationTheta[curAxis] -= 360.0f;
	}
	glutPostRedisplay();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotatef(rotationTheta[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rotationTheta[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rotationTheta[2], 0.0f, 0.0f, 1.0f);
	egg();
	glFlush();
	glutSwapBuffers();
}

void MyInit()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	GLfloat aspectRatio;
	if (vertical == 0) {
		vertical = 1;
	}
	glViewport(0, 0, horizontal, vertical);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	if (horizontal <= vertical) {
		glOrtho(-7.5, 7.5, -7.5 / aspectRatio, 7.5 / aspectRatio, 10.0, -10.0);
	}
	else
	{
		glOrtho(-7.5 * aspectRatio, 7.5 * aspectRatio, -7.5, 7.5, 10.0, -10.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void generateEggPoints() {
	float x, y, z, uf, vf;
	for (int u = 0; u < N; u++) {
		for (int v = 0; v < N; v++) {
			uf = (float)u / ((float)N-1);
			vf = (float)v / ((float)N-1);
			x = (-90 * pow(uf, 5) + 225 * pow(uf, 4) - 270 * pow(uf, 3) + 180 * pow(uf, 2) - (45 * uf)) * cos(M_PI * vf);
			y = 160 * pow(uf, 4) - 320 * pow(uf, 3) + 160 * pow(uf, 2);
			z = (-90 * pow(uf, 5) + 225 * pow(uf, 4) - 270 * pow(uf, 3) + 180 * pow(uf, 2) - (45 * uf)) * sin(M_PI * vf);
			points[u][v][0] = x;
			points[u][v][1] = y;
			points[u][v][2] = z;
			if (y > height) {
				height = y;
			}
		}
	}
}

void keyboardInput(unsigned char key, int x, int y) {
	if (key == 'r') {
		if (curAxis == axis::x) curAxis = axis::y;
		else if (curAxis == axis::y) curAxis = axis::x;
	}
	if (key == 'p') {
		curEggType = eggType::dot;
	}
	if (key == 'l') {
		curEggType = eggType::wire;
	}
	if (key == 't') {
		curEggType = eggType::triangle;
	}
}

int main()
{
	printf("Podaj N: ");
	scanf_s("%d", &N);
	points = new vector3 * [N];
	for (int i = 0; i < N; i++) {
		points[i] = new vector3[N];
	}
	generateEggPoints();
	glutInitWindowSize(600, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Jajo");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(rotateEgg);
	glutKeyboardFunc(keyboardInput);
	MyInit();
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}