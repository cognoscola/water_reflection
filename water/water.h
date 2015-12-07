//
// Created by alvaregd on 06/12/15.
//
#ifndef WATER_REFLECTION_WATER_H
#define WATER_REFLECTION_WATER_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define DUDV_FILE "/home/alvaregd/Documents/Games/water_reflection/assets/waterDUDV.png"
#define NORMALMAP_FILE "/home/alvaregd/Documents/Games/water_reflection/assets/normalMap.png"

#define DUDV 0
#define NORMAL 1

#define REFLECTION_WIDTH  320
#define REFLECTION_HEIGHT 180

#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 120

#define WATER_VERTEX "/home/alvaregd/Documents/Games/water_reflection/water/water.vert"
#define WATER_FRAGMENT "/home/alvaregd/Documents/Games/water_reflection/water/water.frag"

#define WAVE_SPEED 0.03

struct Water {

    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;
    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;

    GLuint dudvTexture;
    GLuint normalMapTexture;

    GLuint positionVbo;
    GLuint texCoordVbo;
    GLuint vao;

    GLuint shader;

    GLint location_reflectionTexture;
    GLint location_refractionTexture;
    GLint location_dudv;
    GLint location_viewMatrix;
    GLint location_modelMatrix;
    GLint location_projMatrix;
    GLint location_moveFactor;
    GLint location_cameraPosition;
    GLint location_normalMap;
    GLint location_lightColour;
    GLint location_lightPosition;
    GLint location_depthMap;

    mat4 modelMatrix;
    GLfloat waterHeight;
    double moveFactor;
};

void waterInit(Water *water, Hardware *hardware, GLfloat* proj_mat);
void waterLoadTexture(Water* water, const char* name, int type);
void waterCreateVao(Water* water);
GLuint createFrameBuffer();
GLuint createTextureAttachment(int width, int height);
GLuint createDepthTextureAttachment(int width, int height);
GLuint createDepthBufferAttachment(int width, int height);
void unbindCurrentFrameBuffer(Hardware* hardware);
void bindFrameBufer(GLuint frameBuffer, int width, int height);
void waterGetUniforms(Water* water);
void waterUpdate(Water *water);
void waterRender(Water* water, Camera *camera);
void waterCleanUp(Water* water);
#endif //WATER_REFLECTION_WATER_H