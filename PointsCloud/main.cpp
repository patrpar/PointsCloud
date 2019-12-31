#include "render_engine/RenderEngine.h"
#include "points_cloud_engine/PointsCloudEngine.h"
#include "core/core.h"

#include "core/Points.h"
#include "camera_calibration/CameraCalibrator.h"
#include <opencv2/opencv.hpp>



int main(int argc, char ** argv)
{
    ArgsParserProgram parser(argc, argv);

    PointsCloudEngine pointsCloudEngine(parser.params);

	/* CAMERA CALIBRATION */ 
    float squareSizeInMeters = 0.021f;
    int numberOfIntersections = 7;

	pointsCloudEngine.calibrateCamera(numberOfIntersections, squareSizeInMeters, "camera_calibration/data/images/");
	/* PRINT CAMERA MATRIX */
	//for (int i = 0; i < cc->distCoeffs.rows; i++)
	//{
	//	for (int j = 0; j < cc->distCoeffs.cols; j++)
	//	{
	//		std::cout << cc->distCoeffs.at<double>(i, j) << "\t";
	//	}
	//	std::cout << "\n";
	//}

	pointsCloudEngine.compute();

    

    //auto examplePoints = pointsCloudEngine.getExamplePoints();

    ////std::vector<float> exampleVertices = {
    ////-0.5f, -0.5f, 0.0f,
    //// 0.5f, -0.5f, 0.0f,
    //// 0.0f,  0.5f, 0.0f
    ////};

    //std::vector<float> cube = {

    //    0.1f, 0.1f, 0.1f,
    //    0.9f, 0.1f, 0.1f,
    //    0.1f, 0.9f, 0.1f,
    //    0.9f, 0.9f, 0.1f,
    //     0.1f, 0.1f, 0.9f,
    //    0.9f, 0.1f, 0.9f,
    //    0.1f, 0.9f, 0.9f,
    //    0.9f, 0.9f, 0.9f
    //};

    std::vector<float> pointsCloud = pointsCloudEngine.getPointsAsVector6f();

    RenderEngine renderEngine(pointsCloud);


    renderEngine.view();


    return 0;
}