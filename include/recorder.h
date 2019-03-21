#pragma  once

#include <opencv2/opencv.hpp>

class Recorder {
private:
    bool is_recording;
    cv::VideoWriter writer;
public:
    void record();

    void stop();

    bool recording() const;

    void write(cv::InputArray);
};
