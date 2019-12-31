#include "CameraCalibrator.h"
#include <opencv2/features2d.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>

CameraCalibrator::CameraCalibrator(int boardSize, float squareSize, std::string path, bool loadCalbrationFromFile)
{
	this->boardSize = boardSize;
	this->path = path;
	for (int i = 0; i < boardSize; ++i) //chessboard used for calibration has 7x7 intersections
		for (int j = 0; j < boardSize; ++j)
			corners.push_back(cv::Point3f(float(j * squareSize), float(i * squareSize), 0));

	objectCorners.push_back(corners);

    LoadImages();
    imagesCount = static_cast<int>(images.size());

	objectCorners.resize(imagesCount, objectCorners[0]);

	cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

    
    //change that later
    bool isCalibrated = false;

    if (loadCalbrationFromFile) {
        isCalibrated = loadCameraCalibration("camera_calibration/data/calibrationData"); //checks if there is calibration data stored already
        if (!isCalibrated) {
            std::cout << "No default calibration data, force calibration!" << std::endl;
        }
    }
	
    


	if (!isCalibrated) {
		calibrateCamera();
		saveCameraCalibration("camera_calibration/data/calibrationData");
	}
}

void CameraCalibrator::LoadImages()
{
	cv::Mat image;
    std::vector<std::string> imageNames;
   
    
    std::string pathToImages = "camera_calibration/data/images";
    for (const auto& entry : std::filesystem::directory_iterator(pathToImages)) {
        imageNames.push_back(entry.path().string());
    }

    for (const auto& name : imageNames) 
	{
        image = cv::imread(name);
		images.push_back(image);
	}

}

void CameraCalibrator::getChessboardCorners(bool showResults)
{
    int i = 0;
	for (std::vector<cv::Mat>::iterator iter = images.begin(); iter != images.end(); iter++) {
		std::vector<cv::Point2f> pointBuf;
		bool found = cv::findChessboardCorners(*iter, cv::Size(boardSize, boardSize), pointBuf, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

        std::cout << "corners: image:" << i++ << std::endl;

		if (found) {
			allFoundCorners.push_back(pointBuf);
            std::cout << "found" << std::endl;
		}

		if (showResults) {
			cv::drawChessboardCorners(*iter, cv::Size(boardSize, boardSize), pointBuf, found);
			cv::imshow("Looking for corners", *iter);
		}
	}
}

void CameraCalibrator::calibrateCamera()
{
	
	getChessboardCorners(false);

	cv::calibrateCamera(objectCorners,
		allFoundCorners,
		cv::Size(boardSize, boardSize),
		cameraMatrix,
		distCoeffs,
		rvecs,
		tvecs,
		0,
		cv::TermCriteria(
			cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, DBL_EPSILON));
}

bool CameraCalibrator::loadCameraCalibration(std::string name)
{
	std::ifstream inStream(name);
	std::string line;
	if (inStream)
	{
		int rows, cols;
		if (std::getline(inStream, line)) {
			std::istringstream iss(line);
			if (!(iss >> rows))
			{
				return false;
			}
		}
		else return false;

		if (std::getline(inStream, line)) {
			std::istringstream iss(line);
			if (!(iss >> cols))
			{
				return false;
			}
		}
		else return false;

		cameraMatrix = cv::Mat::zeros(rows, cols, CV_64F);

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				if (std::getline(inStream, line)) {
					std::istringstream iss(line);
					double a;

					if (!(iss >> a))
					{
						return false;
					}
					cameraMatrix.at<double>(r, c) = a;
				}
				else return false;
			}
		}

		if (std::getline(inStream, line)) {
			std::istringstream iss(line);
			if (!(iss >> rows))
			{
				return false;
			}
		}
		else return false;

		if (std::getline(inStream, line)) {
			std::istringstream iss(line);
			if (!(iss >> cols))
			{
				return false;
			}
		}
		else return false;

		distCoeffs = cv::Mat::zeros(rows, cols, CV_64F);

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				if (std::getline(inStream, line)) {
					std::istringstream iss(line);
					double a;

					if (!(iss >> a))
					{
						return false;
					}
					distCoeffs.at<double>(r, c) = a;
				}
				else return false;
			}
		}
		return true;
	}
	return false;
}

bool CameraCalibrator::saveCameraCalibration(std::string name)
{
	std::ofstream outStream(name);
	if (outStream)
	{
		outStream << cameraMatrix.rows << std::endl;
		outStream << cameraMatrix.cols << std::endl;
		for (int r = 0; r < cameraMatrix.rows; r++) {
			for (int c = 0; c < cameraMatrix.cols; c++) {
				double value = cameraMatrix.at<double>(r, c);
				outStream << value << std::endl;
			}
		}

		outStream << distCoeffs.rows << std::endl;
		outStream << distCoeffs.cols << std::endl;
		for (int r = 0; r < distCoeffs.rows; r++) {
			for (int c = 0; c < distCoeffs.cols; c++) {
				double value = distCoeffs.at<double>(r, c);
				outStream << value << std::endl;
			}
		}
		outStream.close();
		return true;
	}
	outStream.close();
	return false;
}
