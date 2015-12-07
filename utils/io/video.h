//
// Created by alvaregd on 07/12/15.
//

#ifndef WATER_REFLECTION_VIDEO_H
#define WATER_REFLECTION_VIDEO_H

#include <platform/glfw_launcher.h>

#define SCREENSHOT_FILE "/home/alvaregd/Documents/Games/water_reflection/build/screenshot.png"

struct Video{
    unsigned char* g_video_memory_start = NULL;
    unsigned char* g_video_memory_ptr = NULL;
    int g_video_seconds_total = 2;
    int g_video_fps = 25;

    //timers
    bool dump_video = false;
    double video_timer = 0.0; // time video has been recording
    double video_dump_timer = 0.0; // timer for next frame grab
    double frame_time = 0.04; // 1/25 seconds of time

};

void reserve_video_memory (Video* video, Hardware* hardware);
void grab_video_frame (Video* video, Hardware* hardware) ;
bool dump_video_frame (Video* video, Hardware* hardware) ;
bool dump_video_frames (Video* video, Hardware* hardware) ;
bool screencapture (Hardware* hardware) ;

bool videoUpdateTimer(Video* video,double* elapsed_seconds );

#endif //WATER_REFLECTION_VIDEO_H
