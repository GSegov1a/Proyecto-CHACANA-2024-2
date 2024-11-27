#ifndef VIDEO_PROCESSING_H
#define VIDEO_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <vector>

// Declaración de funciones
cv::Mat processFrameDifference(const cv::Mat& prev_gray, const cv::Mat& gray, int luminosity_threshold, int edge_margin);
void detectLuminosityChanges(const cv::Mat& frame, const cv::Mat& bright_areas, int min_movement_size, int edge_margin, bool detection_made);

#endif // VIDEO_PROCESSING_H
