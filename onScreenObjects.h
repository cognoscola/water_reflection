//
// Created by alvaregd on 27/11/15.
//

#ifndef SIMPLE_LEVEL_EDITOR_ONSCREENOBJECTS_H
#define SIMPLE_LEVEL_EDITOR_ONSCREENOBJECTS_H

#include <GL/glew.h>


struct Grid{

    GLuint vao;
    GLuint vertexVbo;
    GLuint colourVbo;
    GLuint colourAttributeIndex;
    int numberOfLines;
    GLfloat heightValue;
};

#endif //SIMPLE_LEVEL_EDITOR_ONSCREENOBJECTS_H
