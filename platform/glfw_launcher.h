//
// Created by alvaregd on 27/11/15.
// Describes platform related functions
//

#ifndef SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H
#define SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <utils/log_utils/logger.h>

struct Window {
    GLFWwindow* window;
    GLFWmonitor *mon;
    const GLFWvidmode* vmode;
};

/** Create a new opengl context */
bool start_gl(Window * hardware);

/** error callback function */
void glfw_error_callback(int error, const char* description);


#endif //SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H
