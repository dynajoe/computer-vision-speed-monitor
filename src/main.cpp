#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <ctime>
#include <tracked_object.h>
#include <functional>
#include <fmt/core.h>
#include <recorder.h>

cv::Mat frame;

int keyboard;

void processImages(const char* filename);

int main(int argc, char** argv) {
    processImages("/Users/joeandaverde/Downloads/20-23-30-23-driving.mov");
    cv::destroyAllWindows();
    return EXIT_SUCCESS;
}


void processImages(const char* filename) {
    cv::VideoCapture capture("http://192.168.1.142/videostream.cgi?user=development&pwd=development&type=.mjpg");
    //cv::VideoCapture capture(0);
    //cv::VideoCapture capture(filename);

    if (!capture.isOpened()) {
        exit(EXIT_FAILURE);
    }

    Recorder recorder;
    capture.read(frame);

    std::vector<std::vector<cv::Point>> contours;

    cv::Mat current_frame, current_gray;
    cv::Mat mask_frame;
    cv::Mat previous_frame, previous_gray;

    frame.copyTo(previous_frame);

    cv::namedWindow("threshold");
    cv::namedWindow("Frame");

    cv::moveWindow("threshold", 0, 0);
    cv::moveWindow("Frame", 0, 420);

    std::vector<TrackedObject> trackers;
    trackers.reserve(10);

    int frame_count = 0;

    while ((char) keyboard != 'q') {
        int fps = capture.get(cv::CAP_PROP_FPS);
        int position = capture.get(cv::CAP_PROP_POS_MSEC);

        std::cout << "Frame: " << frame_count++ << " FPS: " << fps << " Position: " << position << std::endl;
        cv::Mat threshold_image;
        cv::Mat markup_frame;

        if (!capture.read(current_frame)) {
            capture.read(previous_frame);
            capture.read(current_frame);
            break;
        }

        current_frame.copyTo(markup_frame);

        cvtColor(current_frame, current_gray, CV_BGR2GRAY);
        cvtColor(previous_frame, previous_gray, CV_BGR2GRAY);

        blur(current_gray, current_gray, cv::Size(5, 5));
        blur(previous_gray, previous_gray, cv::Size(5, 5));

        absdiff(current_gray, previous_gray, mask_frame);

        threshold(mask_frame, threshold_image, 20, 255, CV_THRESH_BINARY);

        dilate(threshold_image, threshold_image, getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)),
               cv::Point(-1, -1), 5);

        //erode(threshold_image, threshold_image, getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

        findContours(threshold_image, contours, CV_RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        std::vector<std::vector<cv::Point>> convexHulls(contours.size());

        for (unsigned int i = 0; i < contours.size(); i++) {
            cv::convexHull(cv::Mat(contours[i]), convexHulls[i]);
        }

        using DistanceType = std::pair<double, std::reference_wrapper<TrackedObject>>;

        for (auto& ch : contours) {
            cv::Rect bounding_rect(boundingRect(ch));

            if (bounding_rect.area() < 1000)
                continue;

            if (bounding_rect.width < 150)
                continue;

            if (bounding_rect.width + bounding_rect.x >= 640)
                continue;

            if (bounding_rect.x <= 1)
                continue;

            std::vector<DistanceType> distances;
            distances.reserve(trackers.size());

            std::transform(trackers.begin(), trackers.end(), std::back_inserter(distances), [&](TrackedObject& t) {
                return std::make_pair<double, std::reference_wrapper<TrackedObject>>(t.distance_from(bounding_rect), t);
            });

            auto closest = min_element(
                    distances.begin(), distances.end(), [](DistanceType& x, DistanceType& y) {
                        return x.first < y.first;
                    });

            if (closest == distances.end() || closest->first > 200) {
                trackers.emplace_back(current_frame, bounding_rect);
            } else {
                closest->second.get().update(bounding_rect);
            }
        }

        std::for_each(trackers.begin(), trackers.end(), [&](TrackedObject& t) {
            t.tick();

            std::string speed = fmt::format("{:.0f} mph", t.speed(12.24, fps));

            cv::putText(markup_frame,
                        speed,
                        cv::Point(300, 50), // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                        1.0, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255, 255, 255), // BGR Color
                        1);

            cv::rectangle(markup_frame, t.bounding_box, cv::Scalar(0, 255, 0), 2);
        });

        if (!trackers.empty()) {
            trackers.erase(std::remove_if(
                    trackers.begin(),
                    trackers.end(),
                    [](TrackedObject& t) { return t.is_stale(); }), trackers.end());
            if (!recorder.recording()) {
                recorder.record(fps);
            }

            recorder.write(current_frame);
        } else {
            if (recorder.recording()) {
                recorder.stop();
            }
        }

        imshow("Frame", markup_frame);
        imshow("threshold", threshold_image);

        current_frame.copyTo(previous_frame);
        keyboard = cv::waitKey(1);
    }

    recorder.stop();
    capture.release();
}