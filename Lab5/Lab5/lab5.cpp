// Copyright 2020 Jacek Gawron
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>
#include <ctime>

#include "headers/Egg.h"
#include "headers/Light.h"

std::clock_t start;
Egg egg = Egg(0, 0);
Light light0;
Light light1;
float RLight0 = 15, azimuthLight0 = 0;

int status;
float old_x_pos;
float old_y_pos;
float delta_x;
float delta_y;
float azimuthLight1 = 0.0;
float elevationLight1 = 0.0;
float RLight1 = 10.0;
float pix2angle;

int texturePathsCount = 2;
int curTexture = 0;
const char** texturePaths;

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight,
                     GLint* ImComponents, GLenum* ImFormat) {
#pragma pack(1)
  typedef struct {
    GLbyte idlenght;
    GLbyte colormaptype;
    GLbyte datatypecode;
    unsigned short colormapstart;
    unsigned short colormaplenght;
    unsigned char colormapdepth;
    unsigned short x_origin;
    unsigned short y_origin;
    unsigned short width;
    unsigned short height;
    GLbyte bitsperpixel;
    GLbyte descriptor;
  } TGAHEADER;
#pragma pack(8)
  FILE* pFile;
  TGAHEADER tgaHeader;
  unsigned long lImageSize;
  short sDepth;
  GLbyte* pbitsperpixel = NULL;

  *ImWidth = 0;
  *ImHeight = 0;
  *ImFormat = GL_BGRA_EXT;
  *ImComponents = GL_RGB8;
  pFile = fopen(FileName, "rb");
  if (pFile == NULL) return NULL;

  fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

  *ImWidth = tgaHeader.width;
  *ImHeight = tgaHeader.height;
  sDepth = tgaHeader.bitsperpixel / 8;

  if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 &&
      tgaHeader.bitsperpixel != 32)
    return NULL;

  lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

  pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
  if (pbitsperpixel == NULL) return NULL;
  if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
    free(pbitsperpixel);
    return NULL;
  }

  switch (sDepth) {
    case 3:
      *ImFormat = GL_BGR_EXT;
      *ImComponents = GL_RGB8;
      break;
    case 4:
      *ImFormat = GL_BGRA_EXT;
      *ImComponents = GL_RGB8;
      break;
    case 1:
      *ImFormat = GL_LUMINANCE;
      *ImComponents = GL_LUMINANCE8;
      break;
  }
  fclose(pFile);
  return pbitsperpixel;
}

void setTexture(const char* path) {
  GLbyte* pBytes;
  GLint ImWidth, ImHeight, ImComponents;
  GLenum ImFormat;

  // glEnable(GL_CULL_FACE);
  pBytes = LoadTGAImage(path, &ImWidth, &ImHeight, &ImComponents, &ImFormat);

  glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat,
               GL_UNSIGNED_BYTE, pBytes);
  free(pBytes);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void setLightParameters(int lightNumber, Light light) {
  glLightfv(lightNumber, GL_AMBIENT, light.light_ambient);
  glLightfv(lightNumber, GL_DIFFUSE, light.light_diffuse);
  glLightfv(lightNumber, GL_SPECULAR, light.light_specular);
  glLightfv(lightNumber, GL_POSITION, light.light_position);
  glLightf(lightNumber, GL_CONSTANT_ATTENUATION, light.att_constant);
  glLightf(lightNumber, GL_LINEAR_ATTENUATION, light.att_linear);
  glLightf(lightNumber, GL_QUADRATIC_ATTENUATION, light.att_quadtratic);
}

void switchColors(int lightNum, Light light, GLfloat r, GLfloat g, GLfloat b) {
  GLfloat ambient[] = {0.1 * r, 0.1 * g, 0.1 * b};
  GLfloat diffuse[] = {0.9 * r, 0.9 * g, 0.9 * b};
  GLfloat specular[] = {1.0 * r, 1.0 * g, 1.0 * b};
  light1.light_ambient = ambient;
  light1.light_diffuse = diffuse;
  light1.light_specular = specular;
  setLightParameters(lightNum, light);
}

void changeLightPosition(int lightNum, Light light) {
  if (status == 1) {
    azimuthLight1 += delta_x * pix2angle;
    elevationLight1 += delta_y * pix2angle;
  }
  float azi_rad = azimuthLight1 * M_PI / 180.0;
  float ele_rad = elevationLight1 * M_PI / 180.0;
  float xs = RLight1 * cosf(azi_rad) * cosf(ele_rad);
  float ys = RLight1 * sinf(ele_rad);
  float zs = RLight1 * sinf(azi_rad) * cosf(ele_rad);
  float type = light.light_position[3];
  light.light_position = new GLfloat[]{xs, ys, zs, type};
  setLightParameters(lightNum, light);
}

void renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  changeLightPosition(GL_LIGHT1, light1);
  egg.drawEgg();
  glFlush();
  glutSwapBuffers();
}

void myInit() {
  texturePaths = new const char*[texturePathsCount];
  texturePaths[0] = "textures\\tekstura1.tga";
  texturePaths[1] = "textures\\tekstura.tga";
  setTexture(texturePaths[curTexture]);

  GLfloat mat_ambient[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess = 50.0f;

  light0.light_position = new GLfloat[]{0.0, 0.0, -10.0, 0.0};
  light0.light_ambient = new GLfloat[]{0.1, 0.1, 0.1, 1.0};
  light0.light_diffuse = new GLfloat[]{0.9, 0.9, 0.9, 1.0};
  light0.light_specular = new GLfloat[]{1.0, 1.0, 1.0, 1.0};
  light0.att_constant = 1.0f;
  light0.att_linear = 0.05f;
  light0.att_quadtratic = 0.001f;

  light1.light_position = new GLfloat[]{0.0, 0.0, -10.0, 1.0};
  light1.light_ambient = new GLfloat[]{0.1, 0.0, 0.0, 1.0};
  light1.light_diffuse = new GLfloat[]{0.9, 0.0, 0.0, 1.0};
  light1.light_specular = new GLfloat[]{1.0, 0.0, 0.0, 1.0};
  light1.att_constant = 1.0f;
  light1.att_linear = 0.05f;
  light1.att_quadtratic = 0.001f;

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

  setLightParameters(GL_LIGHT0, light0);
  setLightParameters(GL_LIGHT1, light1);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  // glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

  start = std::clock();
}

void changeSize(GLsizei horizontal, GLsizei vertical) {
  pix2angle = 360.0 / static_cast<float>(horizontal);
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
  } else {
    glOrtho(-7.5 * aspectRatio, 7.5 * aspectRatio, -7.5, 7.5, 10.0, -10.0);
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouseFunction(int btn, int state, int x, int y) {
  if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    old_x_pos = x;
    old_y_pos = y;
    status = 1;
  } else {
    status = 0;
  }
  glutPostRedisplay();
}

void motion(GLsizei x, GLsizei y) {
  delta_x = x - old_x_pos;
  delta_y = old_y_pos - y;
  old_x_pos = x;
  old_y_pos = y;
  glutPostRedisplay();
}

void rotateLight() {
  azimuthLight0 += 1.0f;
  if (azimuthLight0 >= 360.0f) {
    azimuthLight0 = 0;
  }
  float x = RLight0 * cosf(azimuthLight0 * (M_PI / 180)) * cosf(0);
  float z = RLight0 * sinf(azimuthLight0 * (M_PI / 180)) * cosf(0);
  GLfloat newPosition[] = {x, 0.0, z, 0.0};
  glLightfv(GL_LIGHT0, GL_POSITION, newPosition);
  glutPostRedisplay();
}

void idle() {
  double duration;
  rotateLight();
  duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
  if (duration > 10.0) {
    curTexture++;
    if (curTexture == texturePathsCount) {
      curTexture = 0;
    }
    setTexture(texturePaths[curTexture]);
    start = std::clock();
  }
}

void keyboardInput(unsigned char key, int x, int y) {
  if (key == 'p') {
    egg.changeEggType(EGG_DOT);
  }
  if (key == 'l') {
    egg.changeEggType(EGG_WIRE);
  }
  if (key == 't') {
    egg.changeEggType(EGG_TRIANGLE);
  }
  if (key == 'r') {
    switchColors(GL_LIGHT1, light1, 1, 0, 0);
  }
  if (key == 'g') {
    switchColors(GL_LIGHT1, light1, 0, 1, 0);
  }
  if (key == 'b') {
    switchColors(GL_LIGHT1, light1, 0, 0, 1);
  }

  glutPostRedisplay();
}

int main(int argc, char** argv) {
  int N;
  printf("Podaj N: ");
  scanf_s("%d", &N);
  egg = Egg(N, EGG_TRIANGLE);
  glutInit(&argc, argv);
  glutInitWindowSize(600, 600);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Oswiecone jajo");
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutKeyboardFunc(keyboardInput);
  glutMouseFunc(mouseFunction);
  glutMotionFunc(motion);
  glutIdleFunc(idle);
  myInit();
  glEnable(GL_DEPTH_TEST);
  glutMainLoop();
  return 0;
}