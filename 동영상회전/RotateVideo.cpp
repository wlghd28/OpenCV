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

// �ð�ݴ����ȸ��
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
		// cap ��ü�κ��� �������� �̹��� �������� �о�鿩 ���� �̹��� ������ �����Ѵ�.
		cap >> orgVideoFrame;

		if (orgVideoFrame.empty())
			break;

		//cv::imshow("camera img", orgVideoFrame);

		// �о���� �̹��� �������� �ð�ݴ� �������� ȸ�� ��Ų�� ���ο� �̹��� ������ �����Ѵ�.
		warpAffine(orgVideoFrame, newVideoFrame, rotateMatrix, videoBox.size());

		//cv::imshow("camera img", newVideoFrame);

		// ȸ����Ų ���ο� �̹��� �������� write ��ü�� �����Ѵ�.
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

