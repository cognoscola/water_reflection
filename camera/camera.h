//
// Created by alvaregd on 02/12/15.
//

#ifndef WATER_REFLECTION_CAMERA_H
#define WATER_REFLECTION_CAMERA_H

#include "utils/math_utils/maths_funcs.h"
#include "GL/gl.h"

struct Camera;
struct Input;

struct Camera{


    float pos[3]; // don't start at zero, or we will be too close
    float yaw = 0.0f; // y-rotation in degrees
    float pitch = 0.0f;
    float signal_amplifier = 0.1f;

    mat4 T;
    mat4 Rpitch;
    mat4 Ryaw;
    mat4 viewMatrix;

    GLint view_mat_location;
    GLint proj_mat_location;

    int pushing; //-1 slowing down, +1 accelerating , 0 = idle
    bool moving; //velocity != 0
    double move_angle;

    vec3 velocity; //actor's velocity
};

#endif //WATER_REFLECTION_CAMERA_H
