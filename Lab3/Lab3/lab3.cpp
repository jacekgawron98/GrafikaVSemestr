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

int status;
float old_x_pos;
float old_y_pos;
float delta_x;
float delta_y;
float pix2angle;
float azimuth = 0.0;
float elevation = 0.0;
float R = 10.0;

void eggDrawDoted() {
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	for (int u = 0; u < N; u++) {
		for (int v = 0; v < N; v++) {
			glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
		}
	}
	glEnd();
}

void eggDrawLines() {
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

void eggDrawTriangles() {
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

void egg() {
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -height / 2, 0.0f);
	if (curEggType == eggType::dot) {
		eggDrawDoted();
	}
	else if (curEggType == eggType::wire) {
		eggDrawLines();
	}
	else if (curEggType == eggType::triangle) {
		eggDrawTriangles();
	}
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if (status == 1) {
		azimuth += delta_x * pix2angle;
		elevation += delta_y * pix2angle;
	}
	float azi_rad = azimuth * M_PI / 180.0;
	float ele_rad = elevation * M_PI / 180.0;
	float xs = R * cosf(azi_rad) * cosf(ele_rad);
	float ys = R * sinf(ele_rad);
	float zs = R * sinf(azi_rad) * cosf(ele_rad);
	gluLookAt(xs, ys, zs, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	egg();
	//glutWireTeapot(3.0);
	glFlush();
	glutSwapBuffers();
}

void MyInit()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1.0, 1.0, 30.0);
	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport(0, (horizontal - vertical) / 2, vertical, vertical);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void generateEggPoints() {
	float x, y, z, uf, vf;
	for (int u = 0; u < N; u++) {
		for (int v = 0; v < N; v++) {
			uf = (float)u / ((float)N - 1);
			vf = (float)v / ((float)N - 1);
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

void mouseFunction(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		old_x_pos = x;
		old_y_pos = y;
		status = 1;
	}
	else {
		status = 0;
	}

	if (btn == 3) {
		R -= 0.1;
	}

	if (btn == 4) {
		R += 0.1;
	}
	glutPostRedisplay();
}

void motion(GLsizei x, GLsizei y) {
	delta_x = x - old_x_pos;
	delta_y = y - old_y_pos;
	old_x_pos = x;
	old_y_pos = y;
	glutPostRedisplay();
}

void keyboardInput(unsigned char key, int x, int y) {
	if (key == 'r') {
		R = 10.0f;
		azimuth = 0.0f;
		elevation = 0.0f;
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
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	printf("Podaj N: ");
	scanf_s("%d", &N);
	points = new vector3 * [N];
	for (int i = 0; i < N; i++) {
		points[i] = new vector3[N];
	}
	generateEggPoints();
	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Lepsze jajo");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(keyboardInput);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(motion);
	MyInit();
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}