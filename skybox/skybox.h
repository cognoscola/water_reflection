//
// Created by alvaregd on 01/12/15.
//

#ifndef SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
#define SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H

#include <GL/glew.h>

#define SKY_VERTEX  "/home/alvaregd/Documents/Games/water_reflection/skybox/skybox.vert"
#define SKY_FRAGMENT "/home/alvaregd/Documents/Games/water_reflection/skybox/skybox.frag"

#define VALUE 100.0f;
#define SKY_MAP_VERTEX_COUNT 36
GLfloat SIZE = VALUE;

GLfloat SKYBOX_VERTICES[] = {
        -SIZE,  SIZE, -SIZE,
        -SIZE, -SIZE, -SIZE,
        SIZE, -SIZE, -SIZE,
        SIZE, -SIZE, -SIZE,
        SIZE,  SIZE, -SIZE,
        -SIZE,  SIZE, -SIZE,

        -SIZE, -SIZE,  SIZE,
        -SIZE, -SIZE, -SIZE,
        -SIZE,  SIZE, -SIZE,
        -SIZE,  SIZE, -SIZE,
        -SIZE,  SIZE,  SIZE,
        -SIZE, -SIZE,  SIZE,

        SIZE, -SIZE, -SIZE,
        SIZE, -SIZE,  SIZE,
        SIZE,  SIZE,  SIZE,
        SIZE,  SIZE,  SIZE,
        SIZE,  SIZE, -SIZE,
        SIZE, -SIZE, -SIZE,

        -SIZE, -SIZE,  SIZE,
        -SIZE,  SIZE,  SIZE,
        SIZE,  SIZE,  SIZE,
        SIZE,  SIZE,  SIZE,
        SIZE, -SIZE,  SIZE,
        -SIZE, -SIZE,  SIZE,

        -SIZE,  SIZE, -SIZE,
        SIZE,  SIZE, -SIZE,
        SIZE,  SIZE,  SIZE,
        SIZE,  SIZE,  SIZE,
        -SIZE,  SIZE,  SIZE,
        -SIZE,  SIZE, -SIZE,

        -SIZE, -SIZE, -SIZE,
        -SIZE, -SIZE,  SIZE,
        SIZE, -SIZE, -SIZE,
        SIZE, -SIZE, -SIZE,
        -SIZE, -SIZE,  SIZE,
        SIZE, -SIZE,  SIZE
};
#endif //SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
