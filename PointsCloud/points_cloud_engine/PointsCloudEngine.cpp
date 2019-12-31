#include "PointsCloudEngine.h"
#include <opencv2/features2d.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <opencv2/core/mat.hpp>

PointsCloudEngine::PointsCloudEngine(ParamsProgram params): params(params)
{
    keyPointDetector = cv::KAZE::create(false,false,0.000001f);
    keyPointMatcher = cv::DescriptorMatcher::create("BruteForce"); 
}

std::vector<Points> PointsCloudEngine::getExamplePoints()
{
    std::vector<Points> examplePointsCloud{ {1.0f, 1.0f, 1.0f}, { 2.0f, 2.0f, 2.0f}, {3.0f, 3.0f, 3.0f} };
    return examplePointsCloud;
}

void PointsCloudEngine::findKeyPoints()
{
    size_t i = 0;
	

    std::cout << "Find key points" << std::endl;

    for (auto& image : imagesBinary) {
        
        keyPointDetector->detect(image, images[i].keyPoints);
        keyPointDetector->compute(image, images[i].keyPoints, images[i].descriptor);

       // cv::Mat mask;
      //  keyPointDetector->detectAndCompute(image, cv::noArray(), images[i].keyPoints, images[i].descriptor);
		
        std::cout << "Key points founds\nImage:" << i << "=> key points:"<< images[i].keyPoints.size() << std::endl;
        ++i;
    }
}

void PointsCloudEngine::compute()
{
    loadImages();
    editImages();
    findKeyPoints();
    connectCommonKeyPoints();
    find3dCoordinates();
}

void PointsCloudEngine::calibrateCamera(int boardSize, float squareSize, std::string path)
{
   auto calibrator = std::make_unique< CameraCalibrator>(boardSize, squareSize, path, params.loadCalibrationDataFromFile);
   cameraMatrixCalibration = calibrator->cameraMatrix;
}

void PointsCloudEngine::loadImages()
{
    std::vector<std::string> imagesNames;

    std::cout << "Loading images" << std::endl;

    for (const auto& entry : std::filesystem::directory_iterator(this->imagesDirectoryPath)) {
       
        std::string imagePath = entry.path().string();
        imagesNames.push_back(imagePath);

        std::cout << "Image path: "<< imagePath << std::endl;
    }

    std::cout << "Found " << imagesNames.size() <<" images" << std::endl;

    imagesBinary.reserve(imagesNames.size());
    for (const auto& imageName : imagesNames) {
        imagesBinary.emplace_back(cv::imread(imageName));
    }
}

void PointsCloudEngine::editImages()
{
    resizeImages();
    saveImages();
   // changeColorsImages();
   
}

void PointsCloudEngine::resizeImages()
{
    std::cout << "Resizing images" << std::endl;

    for (auto& image : imagesBinary) {
        auto imageSizeSmaller = image.size() / (IMAGE_DOWNSAMPLE);
        cv::resize(image, image, imageSizeSmaller);
    }
}

void PointsCloudEngine::changeColorsImages()
{
    std::cout << "Changing images color" << std::endl;

    for (auto& image : imagesBinary) {
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    }
}

void PointsCloudEngine::saveImages()
{

    images.reserve(imagesBinary.size());
    for (auto& image : imagesBinary) {
        images.emplace_back(Image(image));
    }
}

// compare all images
void PointsCloudEngine::connectCommonKeyPoints()
{
    std::cout << "Connecting key points" << std::endl;

    for (uint32_t id_image = 0; id_image < images.size() - 1; ++id_image)
    {
        std::cout << "Image :" << id_image << std::endl;

        std::vector<cv::Point2f> imageMatchedPoints2fForCanvas;

        auto& image = images[id_image];

        for (uint32_t id_secondImage = id_image + 1; id_secondImage < images.size(); ++id_secondImage) {
           // uint32_t id_secondImage = id_image + 1;
            auto& secondImage = images[id_secondImage];

            std::vector<std::vector<cv::DMatch>> keyPointsMatches;
            keyPointMatcher->knnMatch(image.descriptor, secondImage.descriptor, keyPointsMatches, 2);

            // reserve memory for max posible size

            std::vector<int> imageKeyPointsId;
            imageKeyPointsId.reserve(keyPointsMatches.size());
            std::vector<int> secondImageKeyPointsId;
            secondImageKeyPointsId.reserve(keyPointsMatches.size());
            std::vector<cv::Point2f> imageMatchedPoints2f;
            imageMatchedPoints2f.reserve(keyPointsMatches.size());
            std::vector<cv::Point2f> secondImageMatchedPoints2f;
            secondImageMatchedPoints2f.reserve(keyPointsMatches.size());
			
			

            // reduce matches
            for (auto& match : keyPointsMatches) {
                if (match[0].distance < 0.9 * match[1].distance) {
                    imageMatchedPoints2f.emplace_back(image.keyPoints[match[0].queryIdx].pt);
                    secondImageMatchedPoints2f.emplace_back(secondImage.keyPoints[match[0].trainIdx].pt);

                    imageKeyPointsId.emplace_back(match[0].queryIdx);
                    secondImageKeyPointsId.emplace_back(match[0].trainIdx);

                    imageMatchedPoints2fForCanvas.push_back(image.keyPoints[match[0].queryIdx].pt);
                }
            }

           
            if (imageMatchedPoints2f.size() == 0){
                continue;
                std::cout << " WARNING::imageMatchedPoints2f.size() == 0" << std::endl;
              }

            std::vector<uchar> maskElements;
            cv::findFundamentalMat(imageMatchedPoints2f, secondImageMatchedPoints2f, cv::FM_RANSAC, 3.0, 0.99, maskElements);


            for (size_t id_match = 0; id_match < maskElements.size(); ++id_match) {
                if (maskElements[id_match] == 1) {
                    auto& imageKeyPoint = imageKeyPointsId[id_match];
                    auto& secondImageKeyPoint = secondImageKeyPointsId[id_match];
                    
                    // match 2 images together
                    image.keyPointsMatches[imageKeyPoint][id_secondImage] = secondImageKeyPoint;
                    secondImage.keyPointsMatches[secondImageKeyPoint][id_image] = imageKeyPoint;
                }
            }
           
            if (maskElements.size() > 0) {

                cv::Mat canvas;
                canvas.push_back(image.image.clone());
                canvas.push_back(secondImage.image.clone());

                makeCanvasWithMatches(canvas, imageMatchedPoints2f, secondImageMatchedPoints2f, maskElements, static_cast<float>(image.image.rows));

                std::string fileName = "./matches/matched_" + std::to_string(id_image) + "_" + std::to_string(id_secondImage) + ".jpg";
                saveCanvasToFile(canvas, fileName);

            
               // printCanvasToWindow(canvas, fileName);
            }
        }
        // save keyPoints to cacvas

        cv::Mat canvas;
        canvas.push_back(image.image.clone());
       

        for (auto& point : imageMatchedPoints2fForCanvas) {
            cv::circle(canvas, point, 3, cv::Scalar(0, 255, 0));
        }

        std::string fileName = "./key_points/key_points_" + std::to_string(id_image) +".jpg";
        saveCanvasToFile(canvas, fileName);

    }
}

void PointsCloudEngine::find3dCoordinates()
{
    //**************************************************************************************************************
    // Transformation matrix
    // Camera matrix
    // Projection matrix

    double centerX = images[0].image.size().width / 2;
    double centerY = images[0].image.size().height / 2;
    cv::Point2d centerPoint = cv::Point2d(centerX, centerY);
   
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

    
    cameraMatrix = cameraMatrixCalibration;
    cameraMatrix.at<double>(0, 0) = cameraMatrix.at<double>(0, 0) / IMAGE_DOWNSAMPLE/ 3.5f;
    cameraMatrix.at<double>(1, 1) = cameraMatrix.at<double>(1, 1) / IMAGE_DOWNSAMPLE / 3.5f;
    cameraMatrix.at<double>(0, 2) = centerX;
    cameraMatrix.at<double>(1, 2) = centerY;


    std::cout << std::endl << "camera matrix:" << std::endl << cameraMatrix << std::endl << std::endl;
    images[0].transformationMatrix = cv::Mat::eye(4, 4, CV_64F);
    images[0].projectionMatrix = cameraMatrix * cv::Mat::eye(3, 4, CV_64F);

    //**************************************************************************************************************

    std::cout << "Finding 3d coordinates" << std::endl;

    for (size_t id_image = 0u; id_image < images.size() - 1; ++id_image) {
       
        std::cout << "Image: " << id_image << std::endl;

        auto& image = images[id_image];

        size_t id_nextImage = id_image + 1;
        auto& nextImage = images[id_nextImage];

        std::vector<cv::Point2f> imageKeyPoints;
        std::vector<cv::Point2f> nextImageKeyPoints;
        std::vector<size_t> keyPointsUsed;
		std::vector<cv::Vec3b> colors;
		
	//	colors.reserve(image.keyPoints.size());
		imageKeyPoints.reserve(image.keyPoints.size());
        nextImageKeyPoints.reserve(image.keyPoints.size());
        keyPointsUsed.reserve(image.keyPoints.size());


        for (size_t id_keyPoint = 0; id_keyPoint < image.keyPoints.size(); ++id_keyPoint) {
            
            //check if is there match between those images
            if (image.keyPointsMatches[id_keyPoint].count(id_nextImage) > 0) {
                size_t id_match = image.keyPointsMatches[id_keyPoint][id_nextImage];

                imageKeyPoints.emplace_back(image.keyPoints[id_keyPoint].pt);
                nextImageKeyPoints.emplace_back(nextImage.keyPoints[id_match].pt);
             //   auto xOnPhoto = static_cast<int>(image.keyPoints[id_keyPoint].pt.x);
            //    auto yOnPhoto = static_cast<int>(image.keyPoints[id_keyPoint].pt.y);
				//colors.emplace_back(image.image.at<cv::Vec3b>(cv::Point(xOnPhoto, yOnPhoto)));

                keyPointsUsed.emplace_back(id_keyPoint);
            }
        }
       
        // esential matrix
        cv::Mat mask;
        cv::Mat essentialMatrix = cv::findEssentialMat(nextImageKeyPoints, imageKeyPoints, cameraMatrix, cv::RANSAC, 0.999, 1.0, mask);

        cv::Mat recoveredRelativeRotation;
        cv::Mat recoveredRelativeTranslation;
        cv::recoverPose(essentialMatrix, nextImageKeyPoints, imageKeyPoints, cameraMatrix, recoveredRelativeRotation, recoveredRelativeTranslation, mask);

        // Transformation matrix
        nextImage.transformationMatrix = image.transformationMatrix * getTransformationMatrix(recoveredRelativeRotation, recoveredRelativeTranslation);
       
        // Projection matrix
        nextImage.projectionMatrix = getProjectionMatrix(nextImage.transformationMatrix, cameraMatrix);


        // Triangulate Points
        cv::Mat points4D;
        cv::triangulatePoints(image.projectionMatrix, nextImage.projectionMatrix, imageKeyPoints, nextImageKeyPoints, points4D);

        // Scale points
        // by ratio of distance between paring 3d points

        if (id_image > 0) {
           scalePoints(recoveredRelativeRotation, recoveredRelativeTranslation, outputPoints, points4D, image, nextImage, keyPointsUsed, mask, id_image, id_nextImage, cameraMatrix, imageKeyPoints, nextImageKeyPoints);
        }
       
       // extract only good points 

        for (int iterator_keyPointUsed = 0; iterator_keyPointUsed < keyPointsUsed.size(); ++iterator_keyPointUsed) {
            if (mask.at < uchar>(iterator_keyPointUsed)) {
                size_t id_keyPoint = keyPointsUsed[iterator_keyPointUsed];
                size_t id_match = image.keyPointsMatches[id_keyPoint][id_nextImage];

                cv::Point3f point3D;
                point3D.x = points4D.at<float>(0, iterator_keyPointUsed) / points4D.at<float>(3, iterator_keyPointUsed);
                point3D.y = points4D.at<float>(1, iterator_keyPointUsed) / points4D.at<float>(3, iterator_keyPointUsed);
                point3D.z = points4D.at<float>(2, iterator_keyPointUsed) / points4D.at<float>(3, iterator_keyPointUsed);

                
                // match with existing point
                if (image.keyPointAsPoint3d.count(id_keyPoint) > 0) {

                   size_t id_point3d = image.keyPointAsPoint3d[id_keyPoint];
                   nextImage.keyPointAsPoint3d[id_match] = id_point3d;
                   outputPoints[id_point3d].point += point3D;

                   size_t id_nextImagePoint3d = nextImage.keyPointAsPoint3d[id_match];
                   outputPoints[id_nextImagePoint3d].countSeen++;
                }
                else {
                    // add new point
                    RealPoint newPoint;
                    newPoint.point = point3D;
                    newPoint.countSeen = 2;


                    // GET COLORS
                    auto keyPoints_tmp = image.keyPoints[id_keyPoint];
                    auto xOnPhoto = static_cast<int>(keyPoints_tmp.pt.x);
                    auto yOnPhoto = static_cast<int>(keyPoints_tmp.pt.y);
                    auto intensity = image.image.at<cv::Vec3b>(yOnPhoto, xOnPhoto);
                    uchar blue = intensity.val[0];
                    uchar green = intensity.val[1];
                    uchar red = intensity.val[2];
                    newPoint.color.val[0] = red;
                    newPoint.color.val[1] = green;
                    newPoint.color.val[2] = blue;

                 //   std::cout << "red: " << static_cast<int>(newPoint.color.val[0]);
                  //  std::cout << "   green: " << static_cast<int>(newPoint.color.val[1]);
                  //  std::cout << "   blue: " << static_cast<int>(newPoint.color.val[2]) << std::endl;

                    outputPoints.push_back(newPoint);
                    image.keyPointAsPoint3d[id_keyPoint] = outputPoints.size() - 1;
                    nextImage.keyPointAsPoint3d[id_match] = outputPoints.size() - 1;
                }
            }
        }
    }

    for (auto& point : outputPoints) {
        if (point.countSeen >= MIN_LANDMARK_SEEN) {
            point.point /= (point.countSeen - 1);
        }
    }
}

void PointsCloudEngine::makeCanvasWithMatches(cv::Mat& canvas, std::vector<cv::Point2f>& imageMatchedPoints2f, std::vector<cv::Point2f>& secondImageMatchedPoints2f, const std::vector<uchar>& maskElements, float imageHeight)
{
    for (size_t id_match = 0; id_match < maskElements.size(); ++id_match) {
        if (maskElements[id_match] == 1) {

            cv::line(canvas,
                imageMatchedPoints2f[id_match],
                secondImageMatchedPoints2f[id_match] + cv::Point2f(0,imageHeight),
                cv::Scalar(0, 255, 0),
                2);

        }
    }
}

void PointsCloudEngine::saveCanvasToFile(cv::Mat& canvas, std::string& filename)
{
    std::vector<int> compression_params = { cv::IMWRITE_JPEG_QUALITY, 100 };
    cv::imwrite(filename, canvas, compression_params);
    cv::waitKey(1);
}

void PointsCloudEngine::printCanvasToWindow(cv::Mat& canvas, std::string& filename)
{
    editCanvas(canvas);
    cv::imshow(filename, canvas);
    cv::waitKey(1);
}

void PointsCloudEngine::editCanvas(cv::Mat& canvas)
{
    cv::resize(canvas, canvas, canvas.size() / IMAGE_DOWNSAMPLE);
}

cv::Mat PointsCloudEngine::getTransformationMatrix(const cv::Mat& relativeRotation, const cv::Mat& relativeTranslation)
{
    cv::Mat transformMatrix = cv::Mat::eye(4, 4, CV_64F);
    relativeRotation.copyTo(transformMatrix(cv::Range(0, 3), cv::Range(0, 3)));
    relativeTranslation.copyTo(transformMatrix(cv::Range(0, 3), cv::Range(3, 4)));
    return transformMatrix;
}

cv::Mat PointsCloudEngine::getProjectionMatrix(const cv::Mat& transformationMatrix, cv::Mat& cameraMatrix)
{
    cv::Mat rotation = transformationMatrix(cv::Range(0, 3), cv::Range(0, 3));
    cv::Mat transformation = transformationMatrix(cv::Range(0, 3), cv::Range(3, 4));

    cv::Mat projectionMatrix(3, 4, CV_64F);
    projectionMatrix(cv::Range(0, 3), cv::Range(0, 3)) = rotation.t();
    projectionMatrix(cv::Range(0, 3), cv::Range(3, 4)) = -rotation.t() * transformation;
    projectionMatrix = cameraMatrix * projectionMatrix;

    return projectionMatrix;
}

double PointsCloudEngine::calculateScale(const std::vector<cv::Point3f>& newPoints, const std::vector<cv::Point3f>& existingPoints, size_t id_image)
{
    double scale = 0.0;
    size_t count = 0;

    for (size_t it = 0; it < newPoints.size() - 1; ++it) {
        for (size_t secondIt = it + 1; secondIt < newPoints.size(); ++secondIt) {
            double s = cv::norm(existingPoints[it] - existingPoints[secondIt]) / cv::norm(newPoints[it] - newPoints[secondIt]);
            scale += s;
            ++count;
        }
    }

    if (count == 0) {
        assert(true);
        std::cout << "count == 0!" << std::endl;
    }

    scale = scale / count;
    std::cout << "image:" << id_image + 1 << " >> image:" << id_image << " scale:" << scale << " count:" << count << std::endl;

    return scale;  
}

void PointsCloudEngine::scalePoints(cv::Mat& relativeRotation,cv::Mat& relativeTranslation, std::vector<RealPoint>& outputPoints, cv::Mat& points4D, Image& image, Image& nextImage, std::vector<size_t>& keyPointsUsed, cv::Mat& mask, size_t id_image, size_t id_nextImage, cv::Mat& cameraMatrix, std::vector<cv::Point2f>& imageKeyPoints,std::vector<cv::Point2f>& nextImageKeyPoints)
{
    cv::Point3f camera;
    camera.x = static_cast<float>(image.transformationMatrix.at<double>(0, 3));
    camera.y = static_cast<float>(image.transformationMatrix.at<double>(1, 3));
    camera.z = static_cast<float>(image.transformationMatrix.at<double>(2, 3));

    std::vector<cv::Point3f> newPoints;
    std::vector<cv::Point3f> existingPoints;
    newPoints.reserve(keyPointsUsed.size());
    existingPoints.reserve(keyPointsUsed.size());


    for (int iterator_keyPoint = 0; iterator_keyPoint < keyPointsUsed.size(); ++iterator_keyPoint) {
        size_t id_keyPoint = keyPointsUsed[iterator_keyPoint];

        if (mask.at<uchar>(iterator_keyPoint)
            && (image.keyPointsMatches[id_keyPoint].count(id_nextImage) > 0)
            && (image.keyPointAsPoint3d.count(id_keyPoint) > 0)) {

            cv::Point3f point3d;
            point3d.x = points4D.at<float>(0, iterator_keyPoint) / points4D.at<float>(3, iterator_keyPoint);
            point3d.y = points4D.at<float>(1, iterator_keyPoint) / points4D.at<float>(3, iterator_keyPoint);
            point3d.z = points4D.at<float>(2, iterator_keyPoint) / points4D.at<float>(3, iterator_keyPoint);

            newPoints.emplace_back(point3d);

            size_t id_point3d = image.keyPointAsPoint3d[id_keyPoint];
            cv::Point3f avgPoint3d = outputPoints[id_point3d].point / (outputPoints[id_point3d].countSeen - 1);
            existingPoints.emplace_back(avgPoint3d);
        }
    }

    if (newPoints.size() == 0) {
        assert(true);
        std::cout << "newPoints.size() == 0" << std::endl;
    }
    else
    {
        relativeTranslation *= calculateScale(newPoints, existingPoints, id_image);
        // Transformation matrix
        nextImage.transformationMatrix = image.transformationMatrix * getTransformationMatrix(relativeRotation, relativeTranslation);
        // Projection matrix
        nextImage.projectionMatrix = getProjectionMatrix(nextImage.transformationMatrix, cameraMatrix);
        // Triangulate Points
        cv::triangulatePoints(image.projectionMatrix, nextImage.projectionMatrix, imageKeyPoints, nextImageKeyPoints, points4D);

    }
}

std::vector<float> PointsCloudEngine::getPointsAsVector6f()
{
    std::vector<float> points;
    points.reserve(outputPoints.size() * 6);

    int downScale = 6;

    for (const auto& landmark : outputPoints) {
        points.emplace_back(-landmark.point.x/ downScale);
        points.emplace_back(-landmark.point.y/ downScale);
        points.emplace_back(landmark.point.z/ downScale);

        //convert to 0f-1f 
        points.emplace_back(static_cast<int>(landmark.color.val[0]) / 255.f);
        points.emplace_back(static_cast<int>(landmark.color.val[1]) / 255.f);
        points.emplace_back(static_cast<int>(landmark.color.val[2]) / 255.f);

        //std::cout << "red: " << static_cast<int>(landmark.color.val[0]) / 256.f << std::endl;

       // std::cout << "r: " << static_cast< int> (landmark.color.val[0]) << std::endl;
        //std::cout << "g: " << static_cast<int> (landmark.color.val[1]) << std::endl;
       // std::cout << "b: " << static_cast<int> (landmark.color.val[2]) << std::endl;
    }

    // Rescale to -1,1
    //// It should be divide by max (x,y,z), but 10 also works
    //for (auto& point : points) {
    //    point = point/6;
    //}


    return points;
}
