// Copyright 2020 Jacek Gawron
#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include "headers/Egg.h"

Egg::Egg(int size, int type) {
    this->size = size;
    changeEggType(type);
    points = new vector3 * [size];
    normals = new vector3 * [size];
    for (int i = 0; i < size; i++) {
        points[i] = new vector3[size];
        normals[i] = new vector3[size];
    }
    generateEgg();
    generateNormals();
}

void Egg::generateEgg() {
    int N = size;
    float x, y, z, uf, vf;
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            uf = static_cast<float>(u) / (static_cast<float>(N) - 1);
            vf = static_cast<float>(v) / (static_cast<float>(N) - 1);
            x = (-90 * pow(uf, 5) + 225 * pow(uf, 4) - 270 * pow(uf, 3) + 180 * pow(uf, 2) - (45 * static_cast<double>(uf))) * cos(M_PI * vf);
            y = 160 * pow(uf, 4) - 320 * pow(uf, 3) + 160 * pow(uf, 2);
            z = (-90 * pow(uf, 5) + 225 * pow(uf, 4) - 270 * pow(uf, 3) + 180 * pow(uf, 2) - (45 * static_cast<double>(uf))) * sin(M_PI * vf);
            points[u][v][0] = x;
            points[u][v][1] = y;
            points[u][v][2] = z;
            if (y > height) {
                height = y;
            }
        }
    }
}

void Egg::generateNormals() {
    int N = size;
    float xu, xv, yu, yv = 0, zu, zv, uf, vf;
    float x, y, z, length;
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            uf = static_cast<float>(u) / (static_cast<float>(N) - 1);
            vf = static_cast<float>(v) / (static_cast<float>(N) - 1);
            xu = (-450 * pow(uf, 4) + 900 * pow(uf, 3) - 810 * pow(uf, 2) + (360 * static_cast<double>(uf)) - 45) * cos(M_PI * vf);
            xv = M_PI * (90 * pow(uf, 5) - 225 * pow(uf, 4) + 270 * pow(uf, 3) - 180 * pow(uf, 2) + (45 * static_cast<double>(uf))) * sin(M_PI * vf);
            yu = 640 * pow(uf, 3) - 960 * pow(uf, 2) + 320 * static_cast<double>(uf);
            zu = (-450 * pow(uf, 4) + 900 * pow(uf, 3) - 810 * pow(uf, 2) + (360 * static_cast<double>(uf)) - 45) * sin(M_PI * vf);
            zv = -M_PI * (90 * pow(uf, 5) - 225 * pow(uf, 4) + 270 * pow(uf, 3) - 180 * pow(uf, 2) + (45 * static_cast<double>(uf))) * cos(M_PI * vf);
            x = yu * zv - zu * yv;
            y = zu * xv - xu * zv;
            z = xu * yv - yu * xv;
            length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

            normals[u][v][0] = x / length;
            normals[u][v][1] = y / length;
            normals[u][v][2] = z / length;
        }
    }
}

void Egg::eggDrawDoted() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int u = 0; u < size; u++) {
        for (int v = 0; v < size; v++) {
            glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
        }
    }
    glEnd();
}

void Egg::eggDrawLines() {
    int N = size;
    glBegin(GL_LINES);
    for (int u = 0; u < size; u++) {
        for (int v = 0; v < size; v++) {
            if (v != size - 1) {
                glVertex3f(points[u][v + 1][0], points[u][v + 1][1], points[u][v + 1][2]);
            } else {
                glVertex3f(points[u][0][0], points[u][0][1], points[u][0][2]);
            }
            glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
        }
    }
    glEnd();
    glBegin(GL_LINES);
    for (int u = 0; u < size; u++) {
        for (int v = 0; v < size; v++) {
            if (u != N - 1) {
                glVertex3f(points[u + 1][v][0], points[u + 1][v][1], points[u + 1][v][2]);
            } else {
                glVertex3f(points[0][v][0], points[0][v][1], points[0][v][2]);
            }
            glVertex3f(points[u][v][0], points[u][v][1], points[u][v][2]);
        }
    }
    glEnd();
}

void Egg::eggDrawTriangles() {
    int N = size;
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
            glNormal3f(normals[u][(v + 1 - N * vdiff)][0],
                normals[u][(v + 1 - N * vdiff)][1],
                normals[u][(v + 1 - N * vdiff)][2]);
            glVertex3f(points[u][(v + 1 - N * vdiff)][0],
                points[u][(v + 1 - N * vdiff)][1],
                points[u][(v + 1 - N * vdiff)][2]);

            glNormal3f(normals[(u + 1 - N * udiff)][v][0],
                normals[(u + 1 - N * udiff)][v][1],
                normals[(u + 1 - N * udiff)][v][2]);
            glVertex3f(points[(u + 1 - N * udiff)][v][0],
                points[(u + 1 - N * udiff)][v][1],
                points[(u + 1 - N * udiff)][v][2]);

            glNormal3f(normals[u][v][0],
                normals[u][v][1],
                normals[u][v][2]);
            glVertex3f(points[u][v][0],
                points[u][v][1],
                points[u][v][2]);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3f(normals[u][v + 1 - N * vdiff][0],
                normals[u][(v + 1 - N * vdiff)][1],
                normals[u][(v + 1 - N * vdiff)][2]);
            glVertex3f(points[u][v + 1 - N * vdiff][0],
                points[u][(v + 1 - N * vdiff)][1],
                points[u][(v + 1 - N * vdiff)][2]);

            glNormal3f(normals[(u + 1 - N * udiff)][v][0],
                normals[(u + 1 - N * udiff)][v][1],
                normals[(u + 1 - N * udiff)][v][2]);
            glVertex3f(points[(u + 1 - N * udiff)][v][0],
                points[(u + 1 - N * udiff)][v][1],
                points[(u + 1 - N * udiff)][v][2]);

            glNormal3f(normals[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][0],
                normals[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][1],
                normals[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][2]);
            glVertex3f(points[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][0],
                points[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][1],
                points[(u + 1 - N * udiff)][(v + 1 - N * vdiff)][2]);
            glEnd();
        }
    }
}

void Egg::drawEgg() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -height / 2, 0.0f);
    if (currentEggType == eggType::dot) {
        eggDrawDoted();
    } else if (currentEggType == eggType::wire) {
        eggDrawLines();
    } else if (currentEggType == eggType::triangle) {
        eggDrawTriangles();
    }
}

void Egg::changeEggType(int type) {
    if (type == EGG_DOT) {
        currentEggType = eggType::dot;
    }
    if (type == EGG_WIRE) {
        currentEggType = eggType::wire;
    }
    if (type == EGG_TRIANGLE) {
        currentEggType = eggType::triangle;
    }
}
