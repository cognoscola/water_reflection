//
// Created by alvaregd on 27/11/15.
//
#ifndef SIMPLE_LEVEL_EDITOR_MAIN_H_H
#define SIMPLE_LEVEL_EDITOR_MAIN_H_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <utils/math_utils/maths_funcs.h>
#include <utils/math_utils/quat_funcs.h>
#include <utils/log_utils/logger.h>
#include <utils/io/shader_loader.h>
#include <array>
#include "onScreenObjects.h"
#include <vector>
#include "camera/camera.h"

#define VERTEX_SHADER "/home/alvaregd/Documents/Games/simple_level_editor/shaders/grid.vert"
#define FRAGMENT_SHADER "/home/alvaregd/Documents/Games/simple_level_editor/shaders/grid.frag"

enum State{

    STATE_POSITION,
    STATE_SCALE,
    STATE_ORIENTATION
};


struct Input{
    bool wPressed;
    bool sPressed;
    bool aPressed;
    bool dPressed;
};

static Grid grid;
static Camera camera;
static Input input;
static State state;

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void calculateViewMatrices(Camera *camera);
static void updateGridHeight(Grid* grid);

void createVertexBufferObject(GLuint *name, size_t size, GLfloat *data);
void createVertexArrayObjet(GLuint* name, GLuint* bufferObject, GLint dimensions);
void setColourMesh(GLuint* vao, GLuint* bufferObject, GLint dimensions, GLuint* attributeIndex);
#endif //SIMPLE_LEVEL_EDITOR_`_H_H
