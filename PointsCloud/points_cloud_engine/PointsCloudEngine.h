#pragma once


#include <vector>
#include <string_view>
#include <string>
#include "../core/Points.h"
#include "../core/core.h"
#include "PointsCloudEngineHelper.h"
#include <opencv2/opencv.hpp>
#include "Image.h"
#include "../camera_calibration/CameraCalibrator.h"
#include "../core/core.h"
#include <opencv2/core/matx.hpp>

struct RealPoint {
	cv::Point3f point;
	int countSeen = 0;
	cv::Vec3b color;
};



class PointsCloudEngine
{
public:
    PointsCloudEngine( ParamsProgram params);
    std::vector<Points> getExamplePoints();
    void findKeyPoints();
	void compute();
	void calibrateCamera(int boardSize, float squareSize, std::string path);
    std::vector<float> getPointsAsVector6f();


private:
    std::vector<Points> cloudPoints;
    std::vector<cv::Mat> imagesBinary;
    std::string imagesDirectoryPath = "points_cloud_engine/images";
    cv::Ptr<cv::KAZE> keyPointDetector;
    cv::Ptr<cv::DescriptorMatcher> keyPointMatcher;
    std::vector<Image> images;
	std::vector<RealPoint> outputPoints;
    ParamsProgram params;
    bool defaultCameraCalibration = true;
    cv::Mat cameraMatrixCalibration;
    const int IMAGE_DOWNSAMPLE = 2;
    const int MIN_LANDMARK_SEEN = 3;
	

private:
    void loadImages();
    void editImages();
    void resizeImages();
    void changeColorsImages();
    void saveImages();
    void connectCommonKeyPoints();
	void find3dCoordinates();
    void makeCanvasWithMatches(cv::Mat& canvas, std::vector<cv::Point2f> &imgMatches2f, std::vector<cv::Point2f> &secImgMatches2f, const std::vector<uchar>& maskElements, float imageHeight);
    void saveCanvasToFile(cv::Mat& canvas, std::string& filename);
    void printCanvasToWindow(cv::Mat& canvas,std::string& filename);
    void editCanvas(cv::Mat& canvas);
    cv::Mat getTransformationMatrix(const cv::Mat& relativeRotation, const cv::Mat& relativeTranslation);
    cv::Mat getProjectionMatrix(const cv::Mat& transformationMatrix, cv::Mat& cameraMatrix);
    double  calculateScale(const std::vector<cv::Point3f>& newPoints, const std::vector<cv::Point3f>& existingPoints, size_t id_image);

    void scalePoints(cv::Mat& relativeRotation, cv::Mat& relativeTranslation, std::vector<RealPoint>& outputPoints, cv::Mat& points4D, Image& image, Image& nextImage, std::vector<size_t>& keyPointsUsed, cv::Mat& mask, size_t id_image, size_t id_nextImage, cv::Mat& cameraMatrix, std::vector<cv::Point2f>& imageKeyPoints, std::vector<cv::Point2f>& nextImageKeyPoints);
   
};
