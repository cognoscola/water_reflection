#include "main.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <utils/io/stb_image.h>
#include <skybox/skybox.h>
#include <platform/glfw_launcher.h>

#define MESH_FILE "/home/alvaregd/Documents/Games/water_reflection/assets/floating_island.obj"
//#define MESH_FILE "/home/alvaregd/Documents/Games/water_reflection/assets/woodcupsmooth.obj"
//#define MESH_FILE "/home/alvaregd/Documents/Games/greyscale_terrain.obj"

#define SKY_BACK "/home/alvaregd/Documents/Games/water_reflection/assets/back.png"
#define SKY_BOTTOM "/home/alvaregd/Documents/Games/water_reflection/assets/bottom.png"
#define SKY_FRONT "/home/alvaregd/Documents/Games/water_reflection/assets/front.png"
#define SKY_LEFT "/home/alvaregd/Documents/Games/water_reflection/assets/left.png"
#define SKY_RIGHT "/home/alvaregd/Documents/Games/water_reflection/assets/right.png"
#define SKY_TOP "/home/alvaregd/Documents/Games/water_reflection/assets/top.png"

#define WATER_VERTEX "/home/alvaregd/Documents/Games/water_reflection/water/water.vert"
#define WATER_FRAGMENT "/home/alvaregd/Documents/Games/water_reflection/water/water.frag"

#define MESH_VERTEX "/home/alvaregd/Documents/Games/water_reflection/mesh/mesh.vert"
#define MESH_FRAGMENT "/home/alvaregd/Documents/Games/water_reflection/mesh/mesh.frag"

#define CUP_TEXTURE "/home/alvaregd/Documents/Games/water_reflection/assets/ao_colour.png"

#define REFLECTION_WIDTH  320
#define REFLECTION_HEIGHT 180

#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 120

static void calculatePitch(GLfloat angle);

void unbindCurrentFrameBuffer(Hardware* hardware) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, hardware->vmode->width, hardware->vmode->height);

}

//call this function to tell opengl to render to our framebuffer object
void bindFrameBufer(GLuint frameBuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

GLuint createTextureAttachment(int width, int height){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, textureID,0);
    return textureID;
}

GLuint createDepthTextureAttachment(int width, int height){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, textureID,0);
    return textureID;
}

GLuint createDepthBufferAttachment(int width, int height){

    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}

GLuint createFrameBuffer(){

    GLuint frameBufferID;
    glGenFramebuffers(1,&frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return frameBufferID;

}

bool load_mesh(const char* fileName, GLuint* vao, int *point_count){

    const aiScene *scene = aiImportFile(fileName, aiProcess_Triangulate);
    if (!scene) {
        fprintf(stderr, "ERROR: reading mesh %s\n", fileName);
        return false;
    }

    printf(" %i animations\n", scene->mNumAnimations);
    printf(" %i Cameras\n", scene->mNumCameras);
    printf(" %i lights\n", scene->mNumLights);
    printf(" %i Materials\n", scene->mNumMaterials);
    printf(" %i Meshes\n", scene->mNumMeshes);
    printf(" %i textures\n", scene->mNumTextures);

    /*get the first mesh */
    const aiMesh* mesh = scene->mMeshes[0];
    printf("  %i vertices in mesh[0]\n", mesh->mNumVertices);

    /**pass back the mesh count */
    *point_count  = mesh->mNumVertices;

    /* generate */
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    /** make the data contiguous */
    GLfloat * points = NULL;
    GLfloat * normals = NULL;
    GLfloat * texcoords = NULL;

    if (mesh->HasPositions()) {
        points = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mVertices[i]);
            points[i * 3 + 0] = (GLfloat)vp->x;
            points[i * 3 + 1] = (GLfloat)vp->y;
            points[i * 3 + 2] = (GLfloat)vp->z;
        }
    }

    if (mesh->HasNormals()) {
        normals = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mNormals[i]);
            normals[i * 3 + 0] = (GLfloat)vp->x;
            normals[i * 3 + 1] = (GLfloat)vp->y;
            normals[i * 3 + 2] = (GLfloat)vp->z;

        }
    }

    if (mesh->HasTextureCoords(0)) {
        texcoords = (GLfloat *) malloc(*point_count * 2 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mTextureCoords[0][i]);
            texcoords[i * 2 + 0] = (GLfloat)vp->x;
            texcoords[i * 2 + 1] = (GLfloat)vp->y;

        }
    }

    /** make vbos*/
    if (mesh->HasPositions()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        free(points);
    }

    if (mesh->HasNormals()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        free(normals);
    }

    if (mesh->HasTextureCoords(0)) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * *point_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        free(texcoords);
    }

    if (mesh->HasTangentsAndBitangents()) {

    }

    aiReleaseImport(scene);
    printf("Mesh loaded\n");
    return true;
}

GLuint loadCubeMap(){

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (int i = 0; i < 6; ++i) {

        const char* filename;
        switch (i) {
            case 0: filename = SKY_RIGHT; break;
            case 1: filename = SKY_LEFT;break;
            case 2: filename = SKY_TOP;break;
            case 3: filename = SKY_BOTTOM;break;
            case 4: filename = SKY_BACK;break;
            case 5: filename = SKY_FRONT;break;
            default:filename= 0;
        }

        if (filename == 0) {
            return -1;
        }

        int x, y, n;
        int force_channels =4;
        unsigned char *image_data = stbi_load(filename , &x, &y, &n, force_channels);
        if (!image_data) {
            fprintf(stderr, "ERROR: could not load %s\n", filename );
        }
        if((x & (x-1)) != 0 || (y &(y - 1)) != 0) {
            fprintf(stderr, "WARNING:texture %s is not a power of 2 dimensiions\n", filename );
        }

        GLenum target = GL_ERROR_REGAL;
        switch (i) {
            case 0: target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;break;
            case 1: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;break;
            case 2: target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;break;
            case 3: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;break;
            case 4: target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;break;
            case 5: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;break;
            default:break;
        }

        if (target == GL_ERROR_REGAL) {
            return (GLuint) -1;
        }

        glTexImage2D(target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        free(image_data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    return texID;
}


GLuint getTextureFromFile() {

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    int x, y, n;
    int force_channels = 4;
    unsigned char *image_data = stbi_load(CUP_TEXTURE, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", CUP_TEXTURE);
    }
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(stderr, "WARNING:texture %s is not a power of 2 dimensions\n", CUP_TEXTURE);
    }
    printf("Found texture, size:%dx%d\n", x,y);


    //flip the image
    int width_in_bytes = x *4;
    unsigned char *top = NULL;
    unsigned char *bottom = NULL;
    unsigned char temp = 0;
    int half_height = y/2;

    for (int row = 0; row < half_height; row++) {
        top = image_data + row * width_in_bytes;
        bottom = image_data + (y - row - 1) * width_in_bytes;
        for (int col = 0; col < width_in_bytes; col++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    free(image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return texID;

}

int main () {

    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;
    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;

    GLuint meshTextureID;

    //start logger system
    assert(restart_gl_log());

    Hardware hardware;
    //create our main window
    assert(start_gl(&hardware));

    GLuint meshVao;
    int pointCount;
    assert(load_mesh(MESH_FILE, &meshVao, &pointCount));

    meshTextureID = getTextureFromFile();
//    meshTextureID = 0;


    grid = {};
    grid.numberOfLines = 100;
    grid.heightValue = 0.0f;

    GLfloat *gridColourData = new GLfloat[100 * 2 * 3];
    {
        int totalVerteces = 100 * 2;
        for (int i = 0; i < totalVerteces; ++i) {
            gridColourData[i * 3] = 0.5f;
            gridColourData[i * 3 + 1] = 0.0f;
            gridColourData[i * 3 + 2] = 0.5f;
        }
    }

    //Create our gridPoints coordinates
    GLfloat *gridVertexData = new GLfloat[grid.numberOfLines * 6];
    {
        for (int i = 0; i < grid.numberOfLines; ++i) {
            //draw the lines parallel to the x axis
            if (i < 50) {
                gridVertexData[i * 6] = i - 25;  //
                gridVertexData[i * 6 + 1] = grid.heightValue;
                gridVertexData[i * 6 + 2] = -100.f;
                gridVertexData[i * 6 + 3] = i - 25;  //
                gridVertexData[i * 6 + 4] = grid.heightValue;
                gridVertexData[i * 6 + 5] = 100.0f;
            }
            //draw the lines parallel to the z axis;
            if (i >= 50) {
                gridVertexData[i * 6] =  0.0f;//
                gridVertexData[i * 6 + 1] = -100.0f;
                gridVertexData[i * 6 + 2] = i - 50 - 25.0f;
                gridVertexData[i * 6 + 3] = 0.0f;//
                gridVertexData[i * 6 + 4] = 100.0f;
                gridVertexData[i * 6 + 5] = i - 50 - 25.0f;
            }
        }
    }

    //create a temp texture
    GLfloat texcoords[] = {
            0.0f,0.0f,
            1.0f,0.0f,
            1.0f,1.0f,
            1.0f,1.0f,
            0.0f,1.0f,
            0.0f,0.0f
    };
    GLfloat reflection_points[] = {
            -0.75f, 0.25f,  0.0f,
            -0.25f, 0.25f,  0.0f,
            -0.25f, 0.75f,  0.0f,
            -0.25f, 0.75f,  0.0f,
            -0.75f, 0.75f,  0.0f,
            -0.75f, 0.25f,  0.0f
    };
    GLfloat refraction_points[] = {
            0.25f, 0.25f,  0.0f,
            0.75f, 0.25f,  0.0f,
            0.75f, 0.75f,  0.0f,
            0.75f, 0.75f,  0.0f,
            0.25f, 0.75f,  0.0f,
            0.25f, 0.25f,  0.0f
    };

    GLuint reflectionVbo = 0;
    glGenBuffers(1, &reflectionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), reflection_points, GL_STATIC_DRAW);

    GLuint refractionVbo = 0;
    glGenBuffers(1, &refractionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, refractionVbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), refraction_points, GL_STATIC_DRAW);

    GLuint water_coords_vbo;
    glGenBuffers(1, &water_coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);

    GLuint waterReflectionVao = 0;
    glGenVertexArrays(1, &waterReflectionVao);
    glBindVertexArray(waterReflectionVao);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint waterRefractionVao = 0;
    glGenVertexArrays(1, &waterRefractionVao);
    glBindVertexArray(waterRefractionVao);
    glBindBuffer(GL_ARRAY_BUFFER, refractionVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, water_coords_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //Create frame buffer object
    reflectionFrameBuffer = createFrameBuffer();
    reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    unbindCurrentFrameBuffer(&hardware);

    refractionFrameBuffer = createFrameBuffer();
    refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    unbindCurrentFrameBuffer(&hardware);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Frame Buffer messed up\n");
    }

    //create our wall items //2 triangles , 3 reflection_points each, 3 coordinate
    glfwSetCursorPosCallback(hardware.window,cursor_position_callback);
    glfwSetInputMode(hardware.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(hardware.window, key_callback);
    glfwSetInputMode(hardware.window,GLFW_STICKY_KEYS, 1);

    GLuint mesh_shader = create_programme_from_files(MESH_VERTEX, MESH_FRAGMENT);
    GLuint water_shader = create_programme_from_files(WATER_VERTEX, WATER_FRAGMENT);
    GLuint object_shader = create_programme_from_files(VERTEX_SHADER, FRAGMENT_SHADER);
    GLuint skybox_shader_program = create_programme_from_files(SKY_VERTEX, SKY_FRAGMENT);
    /* get version info */
    glEnable (GL_DEPTH_TEST); /* enable depth-testing */

    ///Create VAO/VBO for the GRID
    createVertexBufferObject(&grid.vertexVbo, grid.numberOfLines * 6 * sizeof(GLfloat), gridVertexData);
    createVertexBufferObject(&grid.colourVbo, grid.numberOfLines * 6 * sizeof(GLfloat), gridColourData);
    free(gridVertexData);
    free(gridColourData);

    createVertexArrayObjet(&grid.vao, &grid.vertexVbo, 3);
    grid.colourAttributeIndex = 1;
    setColourMesh(&grid.vao, &grid.colourVbo, 3, &grid.colourAttributeIndex);

    //create VAO/VBO for the SKYBOX
    GLuint skyBoxVao;
    GLuint skyBoxVbo;

    createVertexBufferObject(&skyBoxVbo, SKY_MAP_VERTEX_COUNT * 3 * sizeof(GLfloat), SKYBOX_VERTICES);
    createVertexArrayObjet(&skyBoxVao,&skyBoxVbo, 3);

    // camera stuff
#define PI 3.14159265359
#define DEG_TO_RAD (2.0 * PI) / 360.0

    float near = 0.1f;
    float far = 200.0f;
    double fov = 67.0f * DEG_TO_RAD;
    float aspect = (float)hardware.vmode->width /(float)hardware.vmode->height;

    // matrix components
    double range = tan (fov * 0.5f) * near;
    double Sx = (2.0f * near) / (range * aspect + range * aspect);
    double Sy = near / range;
    float Sz = -(far + near) / (far - near);
    float Pz = -(2.0f * far * near) / (far - near);
    GLfloat proj_mat[] = {
            Sx, 0.0f, 0.0f, 0.0f,
            0.0f, Sy, 0.0f, 0.0f,
            0.0f, 0.0f, Sz, -1.0f,
            0.0f, 0.0f, Pz, 0.0f
    };

    camera = {};
    //create view matrix
    camera.pos[0] = 0.0f; // don't start at zero, or we will be too close
    camera.pos[1] = 0.0f; // don't start at zero, or we will be too close
    camera.pos[2] = 0.5f; // don't start at zero, or we will be too close
    camera.T = translate (identity_mat4 (), vec3 (-camera.pos[0], -camera.pos[1], -camera.pos[2]));
    camera.Rpitch = rotate_y_deg (identity_mat4 (), -camera.yaw);
    camera.Ryaw = rotate_y_deg (identity_mat4 (), -camera.yaw);
    camera.viewMatrix = camera.Rpitch * camera.T;

    glUseProgram(object_shader);
    glEnable (GL_CLIP_DISTANCE0);
    camera.view_mat_location = glGetUniformLocation(object_shader, "view");
    camera.proj_mat_location = glGetUniformLocation(object_shader, "proj");
    GLint planeLocation;
    vec4 plane = vec4(0.0f, -1.0f, 0.0f, 15.0f);
    planeLocation= glGetUniformLocation(object_shader, "plane");

    glUniformMatrix4fv(camera.view_mat_location, 1, GL_FALSE, camera.viewMatrix.m);
    glUniformMatrix4fv(camera.proj_mat_location, 1, GL_FALSE, proj_mat);

    glUseProgram(skybox_shader_program);
    GLuint skyboxTexture = loadCubeMap();

    GLint skybox_projection_mat_location = glGetUniformLocation(skybox_shader_program, "projectionMatrix");
    GLint skybox_view_mat_location = glGetUniformLocation(skybox_shader_program, "viewMatrix");

    glUniformMatrix4fv(skybox_projection_mat_location, 1, GL_FALSE, proj_mat);
    glUniformMatrix4fv(skybox_view_mat_location , 1, GL_FALSE, camera.viewMatrix.m);

    glUseProgram(mesh_shader);
    GLint location_meshViewMatrix  = glGetUniformLocation(mesh_shader, "modelViewMatrix");
    GLint location_meshProjMatrix  = glGetUniformLocation(mesh_shader, "projectionMatrix");
    GLint location_clipPlane       = glGetUniformLocation(mesh_shader, "plane");

    glUniform4f(location_clipPlane, 0.0f, -1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(location_meshViewMatrix, 1, GL_FALSE, camera.viewMatrix.m);
    glUniformMatrix4fv(location_meshProjMatrix , 1, GL_FALSE, proj_mat);


    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};
    GLfloat angle = 0.0f;
    mat4 rotation;
    mat4 skyViewMatrix;

    mat4 s = scale(identity_mat4(), vec3(10,10,10));
    mat4 meshMatrix = camera.viewMatrix * s;

    //water stuff
    GLfloat waterheight = 1.0f;
    GLfloat reflectionDistance;

    while(!glfwWindowShouldClose (hardware.window)) {
        glEnable(GL_CLIP_DISTANCE0);
        updateMovement(&camera);
        calculateViewMatrices(&camera);


        //render to the reflection buffer
        bindFrameBufer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        //calcualte the camera's height while rendering the refleciton
        reflectionDistance = 2 * (camera.pos[1] - waterheight);

        printf("DISTANCE: %f CAMPOS: %f\n", reflectionDistance, camera.pos[1]);

        calculatePitch(-camera.pitch);
        calculateViewMatrices(&camera);

        //set the new view matrix @ the shader level
        angle += 0.001f;
        if (angle > 359) angle = 0;
        create_versor(quat, angle, 0.0f, 1.0f, 0.0f);
        quat_to_mat4(rotation.m, quat);
        skyViewMatrix = camera.viewMatrix * rotation;
        skyViewMatrix.m[12] = 0;
        skyViewMatrix.m[13] = 0;
        skyViewMatrix.m[14] = 0;

        meshMatrix =camera.viewMatrix* s;
        glUseProgram(mesh_shader);
        glUniform4f(location_clipPlane, 0.0f, 1.0f, 0.0f, -waterheight);
        glUniformMatrix4fv(location_meshViewMatrix, 1, GL_FALSE, meshMatrix.m);
        glBindVertexArray(meshVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshTextureID);
        glDrawArrays(GL_TRIANGLES, 0, pointCount);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);


        //draw the sky box
        glUseProgram(skybox_shader_program);
        glUniformMatrix4fv(skybox_view_mat_location, 1, GL_FALSE, skyViewMatrix.m);
        glBindVertexArray(skyBoxVao);
        glEnableVertexAttribArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, SKY_MAP_VERTEX_COUNT);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);

        calculatePitch(camera.pitch);
        calculateViewMatrices(&camera);

        //we are done rendering, now
        unbindCurrentFrameBuffer(&hardware);

        //render to the refraction buffer
        bindFrameBufer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.001f;
        if (angle > 359) angle = 0;
        create_versor(quat, angle, 0.0f, 1.0f, 0.0f);
        quat_to_mat4(rotation.m, quat);
        skyViewMatrix = camera.viewMatrix * rotation;
        skyViewMatrix.m[12] = 0;
        skyViewMatrix.m[13] = 0;
        skyViewMatrix.m[14] = 0;
        meshMatrix =camera.viewMatrix* s;
        glUseProgram(mesh_shader);
        glUniform4f(location_clipPlane, 0.0f,-1.0f, 0.0f, waterheight);
        glUniformMatrix4fv(location_meshViewMatrix, 1, GL_FALSE, meshMatrix.m);
        glBindVertexArray(meshVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshTextureID);
        glDrawArrays(GL_TRIANGLES, 0, pointCount);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);


        //draw the sky box
        glUseProgram(skybox_shader_program);
        glUniformMatrix4fv(skybox_view_mat_location, 1, GL_FALSE, skyViewMatrix.m);
        glBindVertexArray(skyBoxVao);
        glEnableVertexAttribArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, SKY_MAP_VERTEX_COUNT);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);

        //we are done rendering the water, now render normally
        unbindCurrentFrameBuffer(&hardware);

        //Render to the default buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CLIP_DISTANCE0);

        //render the mesh
        meshMatrix =camera.viewMatrix* s;
        glUseProgram(mesh_shader);
        glUniformMatrix4fv(location_meshViewMatrix, 1, GL_FALSE, meshMatrix.m);
        glUniform4f(location_clipPlane, 0.0f, -1.0f, 0.0f, 1000.0f);
        glBindVertexArray(meshVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshTextureID);
        glDrawArrays(GL_TRIANGLES, 0, pointCount);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        //draw the sky box
        glUseProgram(skybox_shader_program);
        glUniformMatrix4fv(skybox_view_mat_location, 1, GL_FALSE, skyViewMatrix.m);
        glBindVertexArray(skyBoxVao);
        glEnableVertexAttribArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, SKY_MAP_VERTEX_COUNT);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);

        //render the reflection texture
        glUseProgram(water_shader);
        glBindVertexArray(waterReflectionVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        //render the refraction texture
        glBindVertexArray(waterRefractionVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, refractionTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(hardware.window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(hardware.window, 1);
        }
        glfwSwapBuffers(hardware.window);
    }

    glDeleteBuffers(1, &reflectionVbo);
    glDeleteBuffers(1, &water_coords_vbo);
    glDeleteVertexArrays(1, &waterReflectionVao);

    glDeleteVertexArrays(1, &meshVao);
    glDeleteVertexArrays(1, &skyBoxVao);
    glDeleteBuffers(1, &skyBoxVbo);

    glDeleteFramebuffers(1, &reflectionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteTextures(1, &refractionDepthTexture);

    /* close GL context and any other GLFW resources */
    glfwTerminate();
    return 0;
}

/**
 * Called everytime we press a key on the keyboard
 * in window - the focused window
 * in key    - which key?
 * in scancode
 * in action - One of GFLW_PRESS, GLFW_REPEAT or GLFW_RELEASE
 */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_W:
            input.wPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.wPressed);
            camera.move_angle =  action == GLFW_PRESS ? 0:camera.move_angle;
            break;
        case GLFW_KEY_S:
            input.sPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.sPressed);
            camera.move_angle =  action == GLFW_PRESS ? 180:camera.move_angle;
            break;
        case GLFW_KEY_A:
            input.aPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.aPressed);
            camera.move_angle =  action == GLFW_PRESS ? -90:camera.move_angle;
            break;
        case GLFW_KEY_D:
            input.dPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.dPressed);
            camera.move_angle =  action == GLFW_PRESS ? 90:camera.move_angle;
            break;
        case GLFW_KEY_PAGE_UP:
            if (action == GLFW_PRESS) {
                switch (state) {
                    case STATE_POSITION: updateGridHeight(&grid); break;
                    case STATE_SCALE:break;
                    case STATE_ORIENTATION:break;
                }
            }
            break;
        case GLFW_KEY_PAGE_DOWN:
            if (action == GLFW_PRESS) {
                switch (state) {
                    case STATE_POSITION:updateGridHeight(&grid);break;
                    case STATE_SCALE:break;
                    case STATE_ORIENTATION:break;
                }
            }
            break;

        case GLFW_KEY_UP:
            if(action == GLFW_PRESS || action == GLFW_REPEAT) {
                switch (state) {
                    case STATE_POSITION:
                        break;
                    case STATE_SCALE:
                        break;
                    case STATE_ORIENTATION:
                        break;
                }
            }
            break;
        case GLFW_KEY_DOWN:
            if(action == GLFW_PRESS || action == GLFW_REPEAT)
            {
                switch (state) {
                    case STATE_POSITION:
                        break;
                    case STATE_SCALE:
                        break;
                    case STATE_ORIENTATION:
                        break;
                }
            }
            break;
        case GLFW_KEY_LEFT:
            if(action == GLFW_PRESS || action == GLFW_REPEAT)
            {
                switch (state) {
                    case STATE_POSITION:
                        break;
                    case STATE_SCALE:
                        break;
                    case STATE_ORIENTATION:
                        break;
                }
            }
            break;
        case GLFW_KEY_RIGHT:
            if(action == GLFW_PRESS || action == GLFW_REPEAT)
            {
                switch (state) {
                    case STATE_POSITION:
                        break;
                    case STATE_SCALE:
                        break;
                    case STATE_ORIENTATION:
                        break;
                }
            }
            break;
        case GLFW_KEY_ENTER: {
        }
            break;
        case GLFW_KEY_1:state = STATE_POSITION;   break;
        case GLFW_KEY_2:state = STATE_ORIENTATION;break;
        case GLFW_KEY_3:state = STATE_SCALE;      break;
    }
}


/**
 * Called every time the cursor moves. It is used to calculate the Camera's direction
 * in window - the window holding the cursor
 * in xpos   - the xposition of the cursor on the screen
 * in ypos   - the yposition of the curose on the screen
 */
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {

    GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};

    //calculate pitch
    static double  previous_ypos = ypos;
    double position_y_difference = ypos - previous_ypos;
    previous_ypos = ypos;

    //calculate yaw
    static double previous_xpos = xpos;
    double position_x_difference = xpos - previous_xpos;
    previous_xpos = xpos;

    //reduce signal
    camera.yaw += position_x_difference *camera.signal_amplifier;
    camera.pitch += position_y_difference *camera.signal_amplifier;

    //calculate rotation sequence
//    create_versor(quat, camera.pitch, 1.0f, 0.0f, 0.0f);
//    quat_to_mat4(camera.Rpitch.m, quat);
    calculatePitch(camera.pitch);
    create_versor(quat, camera.yaw, 0.0f, 1.0f, 0.0f);
    quat_to_mat4(camera.Ryaw.m,quat);

}

static void calculatePitch(GLfloat angle){
    GLfloat quat[] = {0.0f,0.0f,0.0f,0.0f};
    create_versor(quat, angle, 1.0f, 0.0f, 0.0f);
    quat_to_mat4(camera.Rpitch.m, quat);
}

/**
 * Change the height of the floor grid
 */
static void updateGridHeight(Grid* grid){

    //Modify the value
    glBindBuffer(GL_ARRAY_BUFFER, grid->vertexVbo);
    GLfloat *data = (GLfloat *) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    if (data != (GLfloat *) NULL) {
        for (int i = 0; i < (grid->numberOfLines*2); ++i) {
            data[i * 3 + 1 ] =grid->heightValue ;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

/**
 * calculate a new View Matrix
 */
static void calculateViewMatrices(Camera *camera){
    camera->T = translate (identity_mat4 (), vec3 (-camera->pos[0], -camera->pos[1], -camera->pos[2]));
    camera->viewMatrix = camera->Rpitch * camera->Ryaw * camera->T;
}

/**
 * Calculate the player's kinematics and render it
 * When we detect a keypress, push becomes 1 (positive acceleration) until we release the key.
 * When key releases pushing becomes -1 (negative acceleration) to indicate that we are slowing down.
 */
static void updateMovement(Camera* camera) {

    //if any of the WASD keys are pressed, the camera will move
    if(input.wPressed || input.sPressed || input.aPressed || input.dPressed) {
        camera->pushing = 1;
    }

    //while we push,
    if (camera->pushing) {
        //set linear motion
        const double maxVelocity = 0.1 * (camera->pushing> 0);
        const double acceleration= camera->pushing>0 ? 0.2:0.1;

        if(camera->move_angle == 90.0f || camera->move_angle == -90.0f) {
            //Player has pressed either straf left or straf right, calculate the direction Vector using cross product
            //of actor's heading and the up direction
            vec3 left = cross(vec3(camera->viewMatrix.m[2],camera->viewMatrix.m[6],camera->viewMatrix.m[10]),
                              vec3(camera->viewMatrix.m[1],camera->viewMatrix.m[5],camera->viewMatrix.m[9]));

            //update the camera's velocity accordingly
            camera->velocity.v[0] =(float)(camera->velocity.v[0] * (1-acceleration) +
                                           ( left.v[0]) * ((camera->move_angle == 90 )? 1:-1) * (acceleration *maxVelocity));
            camera->velocity.v[2] =(float)(camera->velocity.v[2] * (1-acceleration) +
                                           ( left.v[2]) * ((camera->move_angle == 90 )? 1:-1) * (acceleration *maxVelocity));
        }else{
            //player has hit forward (w) or backwards (s). update the velocity in these directions
            camera->velocity.v[0] =(float)(camera->velocity.v[0] * (1-acceleration) +
                                           ( camera->viewMatrix.m[2]) * ((camera->move_angle == 180 )? -1:1) * (acceleration *maxVelocity));
            camera->velocity.v[1] =(float)(camera->velocity.v[1] * (1-acceleration) +
                                           ( camera->viewMatrix.m[6]) * ((camera->move_angle == 180 )? -1:1) * (acceleration *maxVelocity));
            camera->velocity.v[2] =(float)(camera->velocity.v[2] * (1-acceleration) +
                                           ( camera->viewMatrix.m[10]) * ((camera->move_angle == 180 )? -1:1) * (acceleration *maxVelocity));
        }
        camera->moving = true;
    }

    //while we are moving (velocity is nonzero), update the camera's position
    if (camera->moving) {
        camera->pos[0] += -camera->velocity.v[0] *0.3f;
        camera->pos[2] += -camera->velocity.v[2] *0.3f;
        camera->pos[1] += -camera->velocity.v[1] *0.3f;
        if(dot(camera->velocity,camera->velocity) < 1e-9) {
            camera->velocity.v[0] = camera->velocity.v[2] = camera->velocity.v[1] = 0.0f;
            camera->pushing = 0;
            camera->moving = false;
        }
    }
    if(camera->pushing){
        camera->pushing = -1;
    }
}



void createVertexBufferObject(GLuint *name, size_t size, GLfloat *data){

    glGenBuffers (1, name);
    glBindBuffer (GL_ARRAY_BUFFER, *name);
    glBufferData (GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void createVertexArrayObjet(GLuint* name, GLuint* bufferObject, GLint dimensions){

    glGenVertexArrays (1, name);
    glBindVertexArray (*name);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, *bufferObject);
    glVertexAttribPointer (0, dimensions, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

}

void setColourMesh(GLuint* vao, GLuint* bufferObject, GLint dimensions, GLuint* attributeIndex){

    glBindVertexArray (*vao);
    glBindBuffer (GL_ARRAY_BUFFER, *bufferObject);
    glVertexAttribPointer (*attributeIndex, dimensions, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
}



