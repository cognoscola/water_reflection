//
// Created by alvaregd on 06/12/15.
//
//
/**
 * Mesh
 *
 * This object imports data from a .obj and creates a mesh to render to the scene.
 * Unlike an animal object which animates, a mesh object does not.
 */

#ifndef WATER_REFLECTION_MESH_H
#define WATER_REFLECTION_MESH_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>
#include <camera/camera.h>

#define TERRAIN_TEXTURE "./assets/ao_colour.png"
#define MESH_FILE "./assets/floating_island.obj"

#define MESH_VERTEX "./mesh/mesh.vert"
#define MESH_FRAGMENT "./mesh/mesh.frag"


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

/**
 * Initialize a mesh object. This project only has one mesh, so the settings are hardcoded
 * out mesh: the object being initialized
 * in proj_mat: a projection matrix
 */
void meshInit(Mesh* mesh, GLfloat* proj_mat);

/**
 * loads a .obj file into memory
 * in filename: uri of the .obj file containing the mesh information
 * out vao: id of the vao object
 * out point_count: the number of vertices found in the mesh
 */
bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count);

/**
 * load a texture file into memory. The texture file name is hardcoded
 * in this project
 * out mesh: the mesh object to add the texture to
 */
void meshLoadTexture(Mesh* mesh);

/**
 * creates a shader program for the mesh object
 * out mesh: the mesh object that will use the shader program
 */
void meshLoadShaderProgram(Mesh * mesh);

/**
 * fetch uniform locations in the mesh object's shader program
 */
void meshGetUniforms(Mesh* mesh);

/**
 * Renders the mesh object. Call this every frame
 * in mesh: the mesh object to render
 * in planeHeight: the clipping plane height, in case we need to render this object only partially
 */
void meshRender(Mesh* mesh, Camera* camera, GLfloat planeHeight);

/**
 * free memory used by this mesh object
 */
void meshCleanUp(Mesh *mesh);
#endif //WATER_REFLECTION_MESH_H
