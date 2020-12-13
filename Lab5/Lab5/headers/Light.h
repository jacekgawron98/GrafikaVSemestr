// Copyright 2020 Jacek Gawron
#pragma once
#include <windows.h>
#include <gl/gl.h>

struct Light {
    GLfloat* light_position;
    GLfloat* light_ambient;
    GLfloat* light_diffuse;
    GLfloat* light_specular;
    GLfloat att_constant;
    GLfloat att_linear;
    GLfloat att_quadtratic;
};
