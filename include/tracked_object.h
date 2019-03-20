#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

class TrackedObject {
private:
    cv::Ptr<cv::TrackerKCF> tracker;

    int missed_frames = 0;

public:
    cv::Rect2d bounding_box;
    cv::Rect2d previous_box;

    TrackedObject(cv::InputArray frame, cv::Rect bounding_box);

    void update(cv::InputArray frame);

    bool is_stale() const;

    double distance_from(cv::Rect other);

    double speed(int pixels_per_foot);
};