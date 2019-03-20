#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <ctime>
#include <tracked_object.h>
#include <functional>

cv::Mat frame;

int keyboard;

void processImages(const char* filename);

int main(int argc, char** argv) {
    processImages("../../outcpp.avi");

    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}


void processImages(const char* filename) {
    cv::VideoCapture capture("http://192.168.1.142/videostream.cgi?user=admin&pwd=dudeh4x0r&type=.mjpg");
    //cv::VideoCapture capture(0);
    //cv::VideoCapture capture(filename);
    time_t curr_time;
    curr_time = time(NULL);

    std::stringstream output_filename;
    output_filename << "output" << curr_time << ".avi";
    cv::VideoWriter video(output_filename.str(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(640, 480));

    if (!capture.isOpened()) {
        exit(EXIT_FAILURE);
    }

    capture.read(frame);

    std::vector<std::vector<cv::Point>> contours;

    cv::Mat current_frame, current_gray;
    cv::Mat mask_frame;
    cv::Mat previous_frame, previous_gray;

    frame.copyTo(previous_frame);

    cv::namedWindow("threshold");
    cv::namedWindow("Frame");

    cv::moveWindow("threshold", 640, 45);
    cv::moveWindow("Frame", 0, 420);

    std::vector<TrackedObject> trackers;
    trackers.reserve(10);

    while ((char) keyboard != 'q') {
        cv::Mat threshold_image;
        cv::Mat markup_frame;

        if (!capture.read(current_frame)) {
            capture.read(previous_frame);
            capture.read(current_frame);
            continue;
        }

        video.write(current_frame);

        current_frame.copyTo(markup_frame);

        cvtColor(current_frame, current_gray, CV_BGR2GRAY);
        cvtColor(previous_frame, previous_gray, CV_BGR2GRAY);

        blur(current_gray, current_gray, cv::Size(3, 3));
        blur(previous_gray, previous_gray, cv::Size(3, 3));

        absdiff(current_gray, previous_gray, mask_frame);

        threshold(mask_frame, threshold_image, 40, 255, CV_THRESH_BINARY);

        dilate(threshold_image, threshold_image, getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)),
               cv::Point(-1, -1), 5);
        erode(threshold_image, threshold_image, getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

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

            if (bounding_rect.width < 100)
                continue;

            std::vector<DistanceType> distances;
            distances.reserve(trackers.size());

            std::transform(trackers.begin(), trackers.end(), std::back_inserter(distances), [&](TrackedObject& t) {
                return std::make_pair<double, std::reference_wrapper<TrackedObject>>(t.distance_from(bounding_rect), t);
            });

            std::cout << "interesting bbox " << bounding_rect << std::endl;

            auto closest = min_element(
                    distances.begin(), distances.end(), [](DistanceType& x, DistanceType& y) {
                        return x.first < y.first;
                    });

            if (closest == distances.end() || closest->first > 100) {
                trackers.emplace_back(current_frame, bounding_rect);
            }

        }

        std::for_each(trackers.begin(), trackers.end(), [&](TrackedObject& t) {
            t.update(current_frame);

            cv::rectangle(markup_frame, t.bounding_box, cv::Scalar(0, 255, 0), 2);
        });

        std::remove_if(trackers.begin(),
                       trackers.end(),
                       [](TrackedObject& t) { return t.is_stale(); });

        imshow("Frame", markup_frame);
        imshow("threshold", threshold_image);

        current_frame.copyTo(previous_frame);
        keyboard = cv::waitKey(10);
    }

    video.release();
    capture.release();
}