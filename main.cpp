#include "main.h"

#include <skybox/skybox.h>
#include <platform/glfw_launcher.h>
#include <utils/io/stb_image_write.h>
#include <mesh/mesh.h>
#include <water/water.h>

#define PI 3.14159265359
#define DEG_TO_RAD (2.0 * PI) / 360.0

#define SCREENSHOT_FILE "/home/alvaregd/Documents/Games/water_reflection/build/screenshot.png"

static void calculatePitch(GLfloat angle);

unsigned char* g_video_memory_start = NULL;
unsigned char* g_video_memory_ptr = NULL;
int g_video_seconds_total = 2;
int g_video_fps = 25;

void reserve_video_memory (Hardware* hardware) {
    // 480 MB at 800x800 resolution 230.4 MB at 640x480 resolution
    g_video_memory_ptr = (unsigned char*)malloc (
            hardware->vmode->width * hardware->vmode->height * 3 * g_video_fps * g_video_seconds_total
    );
    g_video_memory_start = g_video_memory_ptr;
}

void grab_video_frame (Hardware* hardware) {
    // copy frame-buffer into 24-bit rgbrgb...rgb image
    glReadPixels (
            0, 0, hardware->vmode->width, hardware->vmode->height, GL_RGB, GL_UNSIGNED_BYTE, g_video_memory_ptr
    );
    // move video pointer along to the next frame's worth of bytes
    g_video_memory_ptr += hardware->vmode->width * hardware->vmode->height * 3;
}

bool dump_video_frame (Hardware* hardware) {
    static long int frame_number = 0;
    printf ("writing video frame %li\n", frame_number);
    // write into a file
    char name[1024];
    sprintf (name, "/home/alvaregd/Documents/Games/water_reflection/video/video_frame_%03ld.png", frame_number);

    unsigned char* last_row = g_video_memory_ptr +
                              (hardware->vmode->width * 3 * (hardware->vmode->height - 1));
    if (!stbi_write_png (
            name, hardware->vmode->width, hardware->vmode->height, 3, last_row, -3 * hardware->vmode->width
    )) {
        fprintf (stderr, "ERROR: could not write video file %s\n", name);
        return false;
    }

    frame_number++;
    return true;
}


bool dump_video_frames (Hardware* hardware) {
    // reset iterating pointer first
    g_video_memory_ptr = g_video_memory_start;
    for (int i = 0; i < g_video_seconds_total * g_video_fps; i++) {
        if (!dump_video_frame (hardware)) {
            return false;
        }
        g_video_memory_ptr += hardware->vmode->width * hardware->vmode->height* 3;
    }
    free (g_video_memory_start);
    printf ("VIDEO IMAGES DUMPED\n");
    return true;
}


bool screencapture (Hardware* hardware) {
    unsigned char* buffer = (unsigned char*)malloc (hardware->vmode->width * hardware->vmode->height * 3);
    glReadPixels (0, 0, hardware->vmode->width, hardware->vmode->height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    unsigned char* last_row = buffer + (hardware->vmode->width * 3 * (hardware->vmode->height - 1));
    if (!stbi_write_png (SCREENSHOT_FILE, hardware->vmode->width, hardware->vmode->height, 3, last_row, -3 * hardware->vmode->width)) {
        fprintf (stderr, "ERROR: could not write screenshot file %s\n", SCREENSHOT_FILE);
    }
    free (buffer);
    return true;
}

int main() {

    Hardware hardware;
    //start logger system
    assert(restart_gl_log());
    //create our main window
    assert(start_gl(&hardware));

    reserve_video_memory (&hardware);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Frame Buffer messed up\n");
    }

    //create our wall items //2 triangles , 3 reflection_points each, 3 coordinate
    glfwSetCursorPosCallback(hardware.window,cursor_position_callback);
    glfwSetInputMode(hardware.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(hardware.window, key_callback);
    glfwSetInputMode(hardware.window,GLFW_STICKY_KEYS, 1);


    /* get version info */
    glEnable (GL_DEPTH_TEST); /* enable depth-testing */


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

    Skybox sky;
    skyInit(&sky, proj_mat);

    //SCENE OBJECTS
    Mesh terrain;
    meshInit(&terrain, proj_mat);

    camera = {};
    //create view matrix
    camera.pos[0] = 0.0f; // don't start at zero, or we will be too close
    camera.pos[1] = 10.0f; // don't start at zero, or we will be too close
    camera.pos[2] = 0.5f; // don't start at zero, or we will be too close
    camera.T = translate (identity_mat4 (), vec3 (-camera.pos[0], -camera.pos[1], -camera.pos[2]));
    camera.Rpitch = rotate_y_deg (identity_mat4 (), -camera.yaw);
    camera.Ryaw = rotate_y_deg (identity_mat4 (), -camera.yaw);
    camera.viewMatrix = camera.Rpitch * camera.T;
    glUniformMatrix4fv(camera.view_mat_location, 1, GL_FALSE, camera.viewMatrix.m);
    glUniformMatrix4fv(camera.proj_mat_location, 1, GL_FALSE, proj_mat);

    Water water;
    waterInit(&water, &hardware, proj_mat);

    //load up information of
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //water stuff
    GLfloat reflectionDistance;

    // initialise timers
    bool dump_video = false;
    double video_timer = 0.0; // time video has been recording
    double video_dump_timer = 0.0; // timer for next frame grab
    double frame_time = 0.04; // 1/25 seconds of time

    while(!glfwWindowShouldClose (hardware.window)) {

        //timing calculation
        static double previous_seconds = glfwGetTime ();
        double current_seconds = glfwGetTime ();
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        if(dump_video) {
            // elapsed_seconds is seconds since last loop iteration
            video_timer += elapsed_seconds;
            video_dump_timer += elapsed_seconds;
            // only record 10s of video, then quit
            if (video_timer > g_video_seconds_total) {
                printf("Finished!\n");
                break;
            }
        }

        glEnable(GL_CLIP_DISTANCE0);
        updateMovement(&camera);

        //RENDER THE REFLECTION BUFFER
        bindFrameBufer(water.reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        reflectionDistance = 2 * (camera.pos[1] - water.waterHeight); //camera's height while rendering the refleciton
        calculatePitch(-camera.pitch);
        calculateViewMatrices(&camera);
        camera.viewMatrix.m[13] +=reflectionDistance;
        meshRender(&terrain,&camera,0.5);
        skyUpdate(&sky);
        skyRender(&sky, &camera);
        camera.viewMatrix.m[13] -=reflectionDistance;
        calculatePitch(camera.pitch);
        calculateViewMatrices(&camera);
        unbindCurrentFrameBuffer(&hardware);

        //RENDER THE REFRACTION BUFFER
        bindFrameBufer(water.refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        meshRender(&terrain,&camera, 5.0f); //TODO get the water height
        skyRender(&sky, &camera);
        unbindCurrentFrameBuffer(&hardware);

        //RENDER THE DEFAULT BUFFER
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CLIP_DISTANCE0);
        meshRender(&terrain,&camera,1000.0f);
        skyRender(&sky, &camera);
        waterUpdate(&water);
        waterRender(&water, &camera);

        glfwPollEvents();

        if (GLFW_PRESS == glfwGetKey (hardware.window, GLFW_KEY_P)) {
            dump_video = true;
            printf("Recording...");
        }

        if (GLFW_PRESS == glfwGetKey (hardware.window, GLFW_KEY_SPACE)) {
            assert (screencapture (&hardware));
        }

        if (GLFW_PRESS == glfwGetKey(hardware.window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(hardware.window, 1);
        }

        if (dump_video) { // check if recording mode is enabled
            while (video_dump_timer > frame_time) {
                grab_video_frame (&hardware); // 25 Hz so grab a frame
                video_dump_timer -= frame_time;
            }
        }
        glfwSwapBuffers(hardware.window);
    }

    waterCleanUp(&water);
    meshCleanUp(&terrain);
    skyCleanUp(&sky);

    if(dump_video) {
        dump_video_frames(&hardware);
    }

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

            }
            break;
        case GLFW_KEY_PAGE_DOWN:
            if (action == GLFW_PRESS) {
            }
            break;

        case GLFW_KEY_UP:
            if(action == GLFW_PRESS || action == GLFW_REPEAT) {
            }
            break;
        case GLFW_KEY_DOWN:
            if(action == GLFW_PRESS || action == GLFW_REPEAT)
            {
            }
            break;
        case GLFW_KEY_LEFT:
            if(action == GLFW_PRESS || action == GLFW_REPEAT)
            {
            }
            break;
        case GLFW_KEY_RIGHT:
            if(action == GLFW_PRESS || action == GLFW_REPEAT)
            {
            }
            break;
        case GLFW_KEY_1:break;
        case GLFW_KEY_2:break;
        case GLFW_KEY_3:break;
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




