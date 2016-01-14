//
// Created by alvaregd on 27/11/15.
// Execute platform related calls such as create window, set the size etc..

#include <GL/glew.h>
#include "glfw_launcher.h"

bool start_gl (Window * hardware) {

    const GLubyte* renderer;
    const GLubyte* version;

    gl_log ("starting GLFW %s", glfwGetVersionString ());

    glfwSetErrorCallback (glfw_error_callback);
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    //Create a fullscreen window
    hardware->mon= glfwGetPrimaryMonitor();
    hardware->vmode = glfwGetVideoMode(hardware->mon);
    hardware->window = glfwCreateWindow (hardware->vmode->width, hardware->vmode->height, "Hello World", hardware->mon, NULL);
    if (!hardware->window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }

    //create opengl context
    glfwMakeContextCurrent (hardware->window);

    glewExperimental = GL_TRUE;
    glewInit ();

    // print out version
    renderer = glGetString (GL_RENDERER); /* get renderer string */
    version = glGetString (GL_VERSION); /* version as a string */
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    return true;
}

void glfw_error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

