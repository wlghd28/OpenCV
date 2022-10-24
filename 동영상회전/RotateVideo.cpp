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

// 시계반대방향회전
int RotateVideo(const char* StrVideoCapturePath, const char* StrVideoWritePath, double dbDegree)
{
	int iVideoFPS = 0, iVideoWidth = 0, iVideoHeight = 0;
	cv::VideoCapture cap;
	cv::VideoWriter write;

	cap.open(StrVideoCapturePath);

	if (!cap.isOpened())
	{
		printf("Can't open the %s!!", StrVideoCapturePath);
		return -1;
	}

	iVideoFPS = cap.get(cv::CAP_PROP_FPS);
	iVideoWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	iVideoHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

	cv::Point2f CenterPos((double)iVideoWidth / (double)2, (double)iVideoHeight / (double)2);
	cv::Mat orgVideoFrame;
	cv::Mat newVideoFrame;
	cv::Mat rotateMatrix = cv::getRotationMatrix2D(CenterPos, dbDegree, 1.0);
	cv::Rect videoBox = cv::RotatedRect(CenterPos, cv::Size(iVideoWidth, iVideoHeight), dbDegree).boundingRect();

	rotateMatrix.at<double>(0, 2) += videoBox.width / 2.0 - CenterPos.x;
	rotateMatrix.at<double>(1, 2) += videoBox.height / 2.0 - CenterPos.y;

	write.open(StrVideoWritePath, cv::VideoWriter::fourcc('F', 'M', 'P', '4'),
		iVideoFPS, videoBox.size(), true);

	if (!write.isOpened())
	{
		printf("Can't open the %s!!", StrVideoWritePath);
		return -1;
	}

	while (1)
	{
		// cap 객체로부터 동영상의 이미지 프레임을 읽어들여 원본 이미지 변수에 저장한다.
		cap >> orgVideoFrame;

		if (orgVideoFrame.empty())
			break;

		//cv::imshow("camera img", orgVideoFrame);

		// 읽어들인 이미지 프레임을 시계반대 방향으로 회전 시킨후 새로운 이미지 변수에 저장한다.
		warpAffine(orgVideoFrame, newVideoFrame, rotateMatrix, videoBox.size());

		//cv::imshow("camera img", newVideoFrame);

		// 회전시킨 새로운 이미지 프레임을 write 객체에 저장한다.
		write << newVideoFrame;

		if (cv::waitKey(25) == 27)
			break;

	}

	cap.release();
	write.release();


	return 0;
}

//project main function
int main(int argc, char** argv) 
{
	RotateVideo("test.mp4", "out.mp4", -90);
	return 0;
}

