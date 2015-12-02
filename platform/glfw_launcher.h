//
// Created by alvaregd on 27/11/15.
//

#ifndef SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H
#define SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H

#include <GLFW/glfw3.h>
#include <stdio.h>
#include "hardware.h"
#include <utils/log_utils/logger.h>

extern Hardware hardware;
bool start_gl();
void glfw_error_callback(int error, const char* description);

#endif //SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H
