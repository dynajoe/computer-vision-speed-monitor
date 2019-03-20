#include <tracked_object.h>

TrackedObject::TrackedObject(const cv::_InputArray& frame, cv::Rect bounding_box)
        : tracker(cv::TrackerKCF::create()),
          bounding_box(bounding_box),
          previous_box(bounding_box) {
    std::cout << "constructing a tracked object" << std::endl;

    tracker->init(frame, bounding_box);
}

void TrackedObject::update(cv::InputArray frame) {
    std::cout << "Update: " << this->bounding_box << std::endl;
    this->previous_box = this->bounding_box;

    if (!this->tracker->update(frame, this->bounding_box)) {
        this->missed_frames++;
    }
}

bool TrackedObject::is_stale() const {
    bool stale = this->missed_frames > 10;
    std::cout << "stale " << stale << std::endl;
    return stale;
}

double TrackedObject::distance_from(cv::Rect other) {
    cv::Point a = (this->bounding_box.br() + this->bounding_box.tl()) * 0.5;
    cv::Point b = (other.br() + other.tl()) * 0.5;
    double distance = cv::norm(a - b);
    std::cout << distance << std::endl;
    return distance;
}

double TrackedObject::speed(int pixels_per_foot) {
    return 0;
}




