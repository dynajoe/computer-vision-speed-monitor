#include <tracked_object.h>

TrackedObject::TrackedObject(const cv::_InputArray& frame, cv::Rect bounding_box)
        : bounding_box(bounding_box),
          previous_box(bounding_box) {
    std::cout << "constructing a tracked object" << std::endl;
}

void TrackedObject::update(cv::Rect bounding_box) {
    this->previous_box = this->bounding_box;
    this->bounding_box = bounding_box;
    this->missed_frames = 0;
}

void TrackedObject::tick() {
    this->missed_frames++;
}

bool TrackedObject::is_stale() const {
    return this->missed_frames > 5;
}

double TrackedObject::distance_from(cv::Rect other) const {
    cv::Point a = (this->bounding_box.br() + this->bounding_box.tl()) * 0.5;
    cv::Point b = (other.br() + other.tl()) * 0.5;
    double distance = cv::norm(a - b);

    return distance;
}

double TrackedObject::speed(double pixels_per_foot, int fps) const {
    double pixels_traveled = this->distance_from(this->previous_box);
    double miles_traveled = (pixels_traveled / pixels_per_foot)/ 5280.0;
    double hours = (1.0 / fps) / 3600;

    return miles_traveled / hours;
}




