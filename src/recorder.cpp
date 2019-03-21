
#include <recorder.h>

#include "recorder.h"

void Recorder::record() {
    if (this->is_recording) {
        this->stop();
    }

    time_t curr_time;
    curr_time = time(NULL);
    std::stringstream output_filename;
    output_filename << "output" << curr_time << ".avi";
    this->writer = cv::VideoWriter(output_filename.str(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(640, 480));
    this->is_recording = true;
}

void Recorder::stop() {
    if (this->is_recording) {
        this->writer.release();
    }
}

bool Recorder::recording() const {
    return this->is_recording;
}

void Recorder::write(cv::InputArray frame) {
    if (this->is_recording) {
       this->writer.write(frame);
    }
}
