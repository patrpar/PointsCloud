#pragma once
#include <opencv2/opencv.hpp>

class CameraCalibrator
{
public:
	CameraCalibrator(int boardSize, float squareSize, std::string path, bool loadCalbrationFromFile);
	void calibrateCamera();
	bool loadCameraCalibration(std::string name);
	bool saveCameraCalibration(std::string name);

	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;

private:
	int boardSize;
	int imagesCount;
	std::vector<cv::Point3f> corners;
	std::vector<std::vector<cv::Point3f>> objectCorners;
	std::vector<std::vector<cv::Point2f>> allFoundCorners;
	std::vector<cv::Mat> images;
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;
	std::string path;

private:
	void LoadImages(); //Loads jpg images from specific path, images must have names from 1.jpg to imagesCount.jpg
	void getChessboardCorners(bool showResults);
};