#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

// define WRITE_LOG 1 if you want to have timing logs, 0 otherwise.
#define WRITE_LOG 0

#include "configurations.h"
#include "serverstreamthread.h"
#include "cudadetectionthread.h"
#include "feedaudiopipethread.h"
#include "feedvideopipethread.h"
#include <QThread>
#include <string>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <sys/ioctl.h> // Allows to check if a pipe is ready to be read
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctime>
#include <cmath>
#include <fstream>

class CameraThread : public QThread
{
    Q_OBJECT
public:
    CameraThread();
    ~CameraThread();
    void run();

private:
    ServerStreamThread *server_stream_thread;
    CudaDetectionThread *cuda_detection_thread;
    FeedAudioPipeThread *audio_pipe_thread;
    FeedVideoPipeThread *video_pipe_thread;

    // Semaphores used to syncronize streaming and detection threads
    sem_t sem_audio, sem_video, sem_picture, sem_detection_done,
          sem_camera_frame;

    // Switch that tells if thread is active or not
    bool thread_active;

    // Prevent audio and frames to be subscribed in subsequent iterations
    bool tik_tok = true;

    // Communication pipes used by communicate from producers to consumers
    int mst_audio_pipe, mst_video_pipe, ffmpeg_audio_pipe, ffmpeg_video_pipe;

    // Pipes paths
    std::string mstaudio_pipe_path, mstvideo_pipe_path, ffaudio_pipe_path,
                ffvideo_pipe_path;


    std::string path; // Camera directory path
    std::string file_name; // Used file name
    std::string strvideo_length; // Length of the input file (in seconds)
    std::string command;
    std::string rtsp_url;
    std::string timing; // Used to extract a chunk from file
    std::string color_scale; // Color scale to use
    // Timing utils variables
    long video_length, begin_chunk, end_chunk;
    int begin_h, begin_m, begin_s, end_h, end_m, end_s;
    // Log time variables
    clock_t start_time1, start_time2;

    void captureFromFile();
    void ffmpegJob();
    void defineChunk();
    void createChunk();
    void captureFromScreen();
    void captureFromCamera();
    void setColorScale();
    void changeFrameColorScale();
    void writeClockTime(std::string log_text, clock_t start_time, bool end);
    void intrusionDetection(bool single_frame);
    void resizeAndBlur(bool single_frame);

public slots:
    void notifyAudioToMstCondVar();
    void notifyVideoToMstCondVar();
    void pictureReady();
    void detectionDone();
    void cameraFrameSaved();

signals:
    void writeText(QString);
    void stopStream();
    void setStreamingEnded();
    void takeAScreenPicture();
    void takeACameraPicture();
    void runIntrusionDetection(bool single_frame);
    void saveCameraFrame(cv::Mat frame);
    void imageScaleBlur(std::string frame_path);
};

#endif // CAMERATHREAD_H
