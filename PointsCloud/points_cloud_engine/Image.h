#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>

using id_keyPoint = size_t;
using id_point3d = size_t;
using id_image = size_t;

class Image
{
public:
    Image(cv::Mat img) : image(img) {}

    cv::Mat image;  // TODO do good names, setters, getters, remove unsued variables
    cv::Mat descriptor;
    std::vector<cv::KeyPoint> keyPoints;
	cv::Mat transformationMatrix; // 4x4 pose transformation matrix
	cv::Mat projectionMatrix; // 3x4 projection matrix

    std::unordered_map<id_keyPoint, std::unordered_map<id_image, id_keyPoint>> keyPointsMatches; // maybe map?
    std::unordered_map<id_keyPoint, id_point3d> keyPointAsPoint3d;


};

