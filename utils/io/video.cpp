//
// Created by alvaregd on 07/12/15.
//

#include "video.h"
#include "stb_image_write.h"

void reserve_video_memory (Video* video, Hardware* hardware) {
    // 480 MB at 800x800 resolution 230.4 MB at 640x480 resolution
    video->g_video_memory_ptr = (unsigned char*)malloc (
            hardware->vmode->width * hardware->vmode->height * 3 * video->g_video_fps * video->g_video_seconds_total
    );
    video->g_video_memory_start = video->g_video_memory_ptr;
}

void grab_video_frame (Video* video, Hardware* hardware) {
    // copy frame-buffer into 24-bit rgbrgb...rgb image
    glReadPixels (
            0, 0, hardware->vmode->width, hardware->vmode->height, GL_RGB, GL_UNSIGNED_BYTE, video->g_video_memory_ptr
    );
    // move video pointer along to the next frame's worth of bytes
    video->g_video_memory_ptr += hardware->vmode->width * hardware->vmode->height * 3;
}

bool dump_video_frame (Video* video,  Hardware* hardware) {
    static long int frame_number = 0;
    printf ("writing video frame %li\n", frame_number);
    // write into a file
    char name[1024];
    sprintf (name, "/home/alvaregd/Documents/Games/water_reflection/video/video_frame_%03ld.png", frame_number);

    unsigned char* last_row = video->g_video_memory_ptr +
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


bool dump_video_frames (Video* video,  Hardware* hardware) {
    // reset iterating pointer first
    video->g_video_memory_ptr = video->g_video_memory_start;
    for (int i = 0; i < video->g_video_seconds_total * video->g_video_fps; i++) {
        if (!dump_video_frame (video, hardware)) {
            return false;
        }
        video->g_video_memory_ptr += hardware->vmode->width * hardware->vmode->height* 3;
    }
    free (video->g_video_memory_start);
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

bool videoUpdateTimer(Video* video,double* elapsed_seconds ){

    if(video->dump_video) {
        // elapsed_seconds is seconds since last loop iteration
        video->video_timer += *elapsed_seconds;
        video->video_dump_timer += *elapsed_seconds;
        // only record 10s of video, then quit
        if (video->video_timer > video->g_video_seconds_total) {
            printf("Finished!\n");
            return true;
        }else{
            return false;
        }
    }
    return false;
}
