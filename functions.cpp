#include "functions.h"

#include <opencv2/imgproc.hpp>


std::unordered_map<std::string, FunctionPtr> g_func_map;
std::vector<FunctionData> g_function_data;


void grayscale(cv::Mat& in, cv::Mat& out, std::vector<float>&)
{
    cv::cvtColor(in, out, cv::COLOR_BGR2GRAY);
}

void quantize(cv::Mat& in, cv::Mat& out, std::vector<float>& params)
{
    int val = static_cast<int>(params[0]);
    cv::Mat table(1, 256, CV_8U);

    for (int i = 0; i < 256; i++){
        table.at<uchar>(0, i) = static_cast<uchar>(val * (i / val));
    }

    int rows = in.rows;
    int cols = in.cols * in.channels();

    for (int x = 0; x < rows; x++)
    {
        uchar* ptr_in = in.ptr<uchar>(x);
        uchar* ptr_out = out.ptr<uchar>(x);

        for (int y = 0; y < cols; y++){
            uchar num = table.at<uchar>(0, ptr_in[y]);
            if (num < 0) {num = 0;}
            if (num > 255.0f) {num = 255.0f;}
            ptr_out[y] = static_cast<uchar>(num);
        }
    }
}

void add(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    uchar val = static_cast<uchar>(params[0]);
    int rows = in.rows;
    int cols = in.cols * in.channels();
    uchar* ptr_in;
    uchar* ptr_out;
    float num;

    for (int x = 0; x < rows; x++)
    {
        ptr_in = in.ptr<uchar>(x);
        ptr_out = out.ptr<uchar>(x);

        for (int y = 0; y < cols; y++){
            num = ptr_in[y] + val;
            if (num > 255.0f) {num = 255.0f;}
            ptr_out[y] = static_cast<uchar>(num);
        }
    }
}

void sub(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    uchar val = static_cast<uchar>(params[0]);
    int rows = in.rows;
    int cols = in.cols * in.channels();
    uchar* ptr_in;
    uchar* ptr_out;
    float num;

    for (int x = 0; x < rows; x++)
    {
        ptr_in = in.ptr<uchar>(x);
        ptr_out = out.ptr<uchar>(x);

        for (int y = 0; y < cols; y++){
            num = ptr_in[y] - val;
            if (num < 0.0f) {num = 0.0f;}
            ptr_out[y] = static_cast<uchar>(num);
        }
    }
}

void divide(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    float val = params[0];
    int rows = in.rows;
    int cols = in.cols * in.channels();
    uchar* ptr_in;
    uchar* ptr_out;
    float num;

    for (int x = 0; x < rows; x++)
    {
        ptr_in = in.ptr<uchar>(x);
        ptr_out = out.ptr<uchar>(x);

        for (int y = 0; y < cols; y++){
            num = ptr_in[y] / val;
            ptr_out[y] = static_cast<uchar>(num);
        }
    }
}

void mul(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    float val = params[0];
    int rows = in.rows;
    int cols = in.cols * in.channels();
    uchar* ptr_in;
    uchar* ptr_out;
    float num;

    for (int x = 0; x < rows; x++)
    {
        ptr_in = in.ptr<uchar>(x);
        ptr_out = out.ptr<uchar>(x);

        for (int y = 0; y < cols; y++){
            num = ptr_in[y] * val;
            if (num > 255.0f) {num = 255.0f;}
            ptr_out[y] = static_cast<uchar>(num);
        }
    }
}

void boxBlur(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    int size = static_cast<int>(params[0]);
    float val = 1.0f / (params[0] * params[0]);

    cv::Mat kernel(size, size, CV_32F);
    kernel.setTo(val);
    cv::filter2D(in, out, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
}

void sobelGray(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    cv::Mat sobel_x, sobel_y;
    cv::Mat gray;
    cv::cvtColor(in, gray, cv::COLOR_BGR2GRAY);
    cv::Mat kernel_x = (cv::Mat_<float>(3, 3) << -1, 0, 1,
                                               -2, 0, 2,
                                               -1, 0, 1);

    cv::Mat kernel_y = (cv::Mat_<float>(3, 3) << 1, 2, 1,
                                                 0, 0, 0,
                                                 -1, -2, -1);

    cv::filter2D(gray, sobel_x, CV_32F, kernel_x, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    cv::filter2D(gray, sobel_y, CV_32F, kernel_y, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

    cv::Mat temp = sobel_x.mul(sobel_x) + sobel_y.mul(sobel_y);
    cv::sqrt(temp, out);

    if (params[0] != -1.0f){
        temp = out.clone();
        double thres = static_cast<double>(params[0]);
        cv::threshold(temp, out, thres, 255, cv::THRESH_BINARY);
    }

    out.convertTo(out, CV_8U);
}

void sobelRGB(cv::Mat& in, cv::Mat& out, std::vector<float>& params){
    cv::Mat sobel_x, sobel_y;
    cv::Mat gray;
    cv::cvtColor(in, gray, cv::COLOR_BGR2GRAY);
    cv::Mat kernel_x = (cv::Mat_<float>(3, 3) << -1, 0, 1,
                        -2, 0, 2,
                        -1, 0, 1);

    cv::Mat kernel_y = (cv::Mat_<float>(3, 3) << 1, 2, 1,
                        0, 0, 0,
                        -1, -2, -1);

    cv::filter2D(gray, sobel_x, CV_32F, kernel_x, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    cv::filter2D(gray, sobel_y, CV_32F, kernel_y, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

    cv::Mat temp = sobel_x.mul(sobel_x) + sobel_y.mul(sobel_y);
    cv::sqrt(temp, out);

    if (params[0] != -1.0f){
        temp = out.clone();
        double thres = static_cast<double>(params[0]);
        cv::threshold(temp, out, thres, 255, cv::THRESH_BINARY);
    }

    out.convertTo(out, CV_8U);
}


void build_funcs()
{
    g_func_map["grayscale"] = &grayscale;
    g_function_data.push_back({
        "grayscale",
        "Grayscale",
        {}
    });

    g_func_map["sobel_g"] = &sobelGray;
    g_function_data.push_back({
        "sobel_g",
        "Sobel Gray",
        {
           { "Threshold", 100, -1, 255, 0, 1.0 }
        }
    });

    g_func_map["sobel_rgb"] = &sobelRGB;
    g_function_data.push_back({
        "sobel_rgb",
        "Sobel RGB",
        {
            { "Threshold", 100, -1, 255, 0, 1.0 }
        }
    });

    g_func_map["quantize"] = &quantize;
    g_function_data.push_back({
        "quantize",
        "Quantize",
        {
            { "Divisor", 100, 1, 255, 0, 1.0 }
        }
    });

    g_func_map["add"] = &add;
    g_function_data.push_back({
        "add",
        "Add",
        {
            { "Constant", 100, 1, 255, 0, 1.0 }
        }
    });

    g_func_map["sub"] = &sub;
    g_function_data.push_back({
        "sub",
        "Subtract",
        {
            { "Constant", 100, 1, 255, 0, 1.0 }
        }
    });

    g_func_map["divide"] = &divide;
    g_function_data.push_back({
        "divide",
        "Divide",
        {
            { "Constant", 2, 1, 255, 0, 1.0 }
        }
    });

    g_func_map["mul"] = &mul;
    g_function_data.push_back({
        "mul",
        "Multiply",
        {
            { "Constant", 2, 1, 255, 0, 1.0 }
        }
    });

    g_func_map["boxBlur"] = &boxBlur;
    g_function_data.push_back({
        "boxBlur",
        "Box Blur",
        {
            { "Size", 5, 3, 33, 0, 1.0 }
        }
    });
}
