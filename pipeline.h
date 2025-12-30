#ifndef PIPELINE_H
#define PIPELINE_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "viewer.h"
#include "functions.h"

class Pipeline
{
public:
    Pipeline();

    Viewer* viewer;

    cv::Mat current_image;
    cv::Mat final_image;
    cv::Mat buffer1, buffer2;

    void setImage(cv::Mat img);
    void processImage(std::vector<FunctionState> states);
};

#endif // PIPELINE_H
