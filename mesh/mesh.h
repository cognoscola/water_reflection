//
// Created by alvaregd on 06/12/15.
//

#ifndef WATER_REFLECTION_MESH_H
#define WATER_REFLECTION_MESH_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>
#include <camera/camera.h>

#define TERRAIN_TEXTURE "/home/alvaregd/Documents/Games/water_reflection/assets/ao_colour.png"
#define MESH_FILE "/home/alvaregd/Documents/Games/water_reflection/assets/floating_island.obj"

#define MESH_VERTEX "/home/alvaregd/Documents/Games/water_reflection/mesh/mesh.vert"
#define MESH_FRAGMENT "/home/alvaregd/Documents/Games/water_reflection/mesh/mesh.frag"


//TODO seperate the modelview matrix into two matrices

struct Mesh{

    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;

    GLint location_projection_mat;
    GLint location_view_mat;
    GLint location_model_mat;
    GLint location_clip_plane;

    mat4 modelMatrix;

    int vertexCount;

};

void meshInit(Mesh* mesh, GLfloat* proj_mat);
bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count);
void meshLoadTexture(Mesh* mesh);
void meshLoadShaderProgram(Mesh * mesh);
void meshGetUniforms(Mesh* mesh);
void meshRender(Mesh* mesh, Camera* camera);

#endif //WATER_REFLECTION_MESH_H
