#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>


class TrackedObject {
private:
    int missed_frames = 0;
public:
    cv::Rect2d bounding_box;
    cv::Rect2d previous_box;

    TrackedObject(cv::InputArray frame, cv::Rect bounding_box);

    void update(cv::Rect bounding_box);

    void tick();

    bool is_stale() const;

    double distance_from(cv::Rect other) const;

    double speed(double pixels_per_foot, int fps) const;
};