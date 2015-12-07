// Created by alvaregd on 01/12/15.

#ifndef SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
#define SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H

#include <GL/glew.h>

#define SKY_VERTEX  "/home/alvaregd/Documents/Games/water_reflection/skybox/skybox.vert"
#define SKY_FRAGMENT "/home/alvaregd/Documents/Games/water_reflection/skybox/skybox.frag"

#define SKY_BACK "/home/alvaregd/Documents/Games/water_reflection/assets/back.png"
#define SKY_BOTTOM "/home/alvaregd/Documents/Games/water_reflection/assets/bottom.png"
#define SKY_FRONT "/home/alvaregd/Documents/Games/water_reflection/assets/front.png"
#define SKY_LEFT "/home/alvaregd/Documents/Games/water_reflection/assets/left.png"
#define SKY_RIGHT "/home/alvaregd/Documents/Games/water_reflection/assets/right.png"
#define SKY_TOP "/home/alvaregd/Documents/Games/water_reflection/assets/top.png"

#define VALUE 100.0f;
#define SKY_MAP_VERTEX_COUNT 36

struct Skybox{
    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;
    GLint projection_mat_location;
    GLint view_mat_location;
    GLint model_mat_location;
    mat4 modelMatrix;

    GLfloat angle;
};

void skyInit(Skybox* sky, GLfloat* projection_matrix);
void skyLoadCubeMapTextures(Skybox *sky);
void skyLoadShaderProgram(Skybox *sky);
void skyCreateVbo(Skybox* sky);
void skyCreateVao(Skybox* sky);
void skyGetUniforms(Skybox* sky);
void skyRender(Skybox *sky, Camera* camera);
void skyUpdate(Skybox *sky);

#endif //SIMPLE_LEVEL_EDITOR_SKYBOX_SHADER_H
