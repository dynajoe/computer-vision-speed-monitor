#pragma  once

#include <opencv2/opencv.hpp>

class Recorder {
private:
    bool is_recording;
    cv::VideoWriter writer;
public:
    void record(int fps);

    void stop();

    bool recording() const;

    void write(cv::InputArray);
};
