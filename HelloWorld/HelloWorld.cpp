/*********************************************************
				Heisanbug OpenCV Test
				2020.07.04
				Hello World
				Alta software developer
**********************************************************/

//opencv header file include
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"

void Test()
{
	//create image window
	cv::namedWindow("image", 1);

	//create test Mat, 400 x 400
	cv::Mat testMat = cv::Mat::zeros(200, 850, CV_8UC3);

	//write text
	cv::putText(testMat, "HELLO WORLD!! HEISANBUG", cvPoint(100, 100),
		cv::FONT_HERSHEY_PLAIN, 3, cvScalar(0, 255, 255), 4);

	//show image
	cv::imshow("image", testMat);
	cv::waitKey(0);

	//close all windows
	cv::destroyAllWindows();
}

//project main function
int main(int argc, char** argv) 
{
	Test();
	return 0;
}

