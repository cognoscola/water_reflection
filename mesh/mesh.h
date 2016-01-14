//
// Created by alvaregd on 06/12/15.
//
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


/** holds opengl information about a mesh*/
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

/** load memory, initialize states etc..*/
void meshInit(Mesh* mesh, GLfloat* proj_mat);

bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count);

/** Load and create an opengl texture */
void meshLoadTexture(Mesh* mesh);
void meshLoadShaderProgram(Mesh * mesh);

/** get shader Uniforms **/
void meshGetUniforms(Mesh* mesh);

/** Render to the mesh to the scene*/
void meshRender(Mesh* mesh, Camera* camera, GLfloat planeHeight);

/** free memory etc.. */
void meshCleanUp(Mesh *mesh);
#endif //WATER_REFLECTION_MESH_H
