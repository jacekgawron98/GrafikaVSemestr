#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int l;

float Randomfloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = diff * random;
	return a + r;
}

void SierpinskiCarpet(GLfloat x, GLfloat y, GLfloat size, int level) {
	if (level == 0) {
		float random = Randomfloat(1.0, 3.0/(float) l);
		glBegin(GL_POLYGON);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(x + random, y + random);
		glColor3f(0.0f, 1.0f, 1.0f);
		random = Randomfloat(1.0, 3.0);
		glVertex2f(x + size + random, y + random);
		glColor3f(1.0f, 1.0f, 0.0f);
		random = Randomfloat(1.0, 3.0);
		glVertex2f(x + size + random, y + size + random);
		glColor3f(1.0f, 0.0f, 1.0f);
		random = Randomfloat(1.0, 3.0);
		glVertex2f(x + random, y + size + random);
		glEnd();
		return;
	}
	size = size / 3;
	for (int i = 0; i < 9; i++) {
		if (i != 4) {
			SierpinskiCarpet(x+(i%3)*size, y + (i / 3) * size, size, level - 1);
		}
	}
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 1.0f, 0.0f);
	SierpinskiCarpet(-glutGet(GLUT_WINDOW_WIDTH) / 4, -glutGet(GLUT_WINDOW_HEIGHT) / 4, glutGet(GLUT_WINDOW_WIDTH) / 2, l);
	glFlush();
}

void MyInit(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	GLfloat aspectRatio;
	if (vertical == 0) {
		vertical = 1;
	}
	glutInitWindowSize(900, 900);
	glViewport(0, 0, horizontal, vertical);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	if (horizontal <= vertical) {
		glOrtho(-100.0, 100.0, -100.0 / aspectRatio, 100.0 / aspectRatio, 1.0, -1.0);
	}
	else
	{
		glOrtho(-100.0 * aspectRatio, 100.0 * aspectRatio, -100.0, 100.0, 1.0, -1.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(void)
{
	srand(time(NULL));
	printf("Podaj poziom dywanu: ");
	scanf_s("%d", &l);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutCreateWindow("Pierwszy program w OpenGL");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	MyInit();
	glutMainLoop();
}