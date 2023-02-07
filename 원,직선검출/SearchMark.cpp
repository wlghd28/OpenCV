//opencv header file include
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"

#define CAMERA_DPI 2540


#define HOUGH
//#define HOUGH_ALT

using namespace cv;
using namespace std;

// �Լ� ���� ����
int Test();	// �׽�Ʈ��


// pDistX, pDistY : ���� ������ �ȼ��Ÿ�����
// imagesrc : �̹��� ���
// double dbThreshold_dp // dp = 1 >> �Է� �̹����� ���� �ػ�, dp = 2 >> �Է� �̹����� ���� �ػ�
// double dbThreshold_mindist // ������ ���� �ּҰŸ�
// double dbThreshold_canny // �̹����� ���ȭ �� �� ���̴� �Ӱ� ��
// double dbThreshold_detection // ���� �迭���� �������� ���� �Ӱ谪 (���� Ŀ�� ���� ��Ȯ, ���� ���� ��ȣ)
int GetDistFromCircle
(
	unsigned char* imagesrc,
	int iImageWidth,
	int iImageHeight,
	int* pDistX,
	int* pDistY,
	double dbDp,
	double dbMindist,
	double dbThreshold_canny,
	double dbThreshold_detection,
	double dbminRadius,
	double dbmaxRadius,
	int iIndex
);

//project main function
int main(int argc, char** argv) {

	// �׽�Ʈ�� �Լ�
	//Test();

	int iDistX = 0;
	int iDistY = 0;
	
	Mat srcImage = imread((const char*)"test1_R3.jpg", IMREAD_GRAYSCALE);

	for (int i = 0; i < 10; i++)
	{
#ifdef HOUGH
		GetDistFromCircle((unsigned char*)srcImage.ptr(), 1280, 1024, &iDistX, &iDistY, 1, 9999, 150, 40, 1, 3, i);
#endif
#ifdef HOUGH_ALT
		GetDistFromCircle((unsigned char*)srcImage.ptr(), 1280, 1024, &iDistX, &iDistY, 1.5, 9999, 300, 0.9, 1, 3, i);
#endif
	}

	//GetDistFromCircle("sample02.jpg" , &iDistX, &iDistY, 1, 150, 300, 40);

	return 0;
}

int Test()
{
	// �̹����ҽ� �ҷ��´�.
	Mat srcImage = imread("sample01.jpg", IMREAD_GRAYSCALE);
	if (srcImage.empty()) return -1;

	// �̹��� �߽� ���Ѵ�.
	//printf("�̹��� �߽� x,y : %d, %d\n", (int)(srcImage.cols / 2), (int)(srcImage.rows / 2));

	// HoughLines() �Ű����� ����
	// src : �Է��� �̹��� ����, Edge detect �� �̹����� �Է��ؾ� ��
	// dst : ������ȯ ���� ���� ������ ������ Array
	// rho : ����� �ȼ�(�Ű� ����)�� �ػ�, �׳� 1�� ����ϸ� ��. (��ȯ�� �׷�������, ������ ���������� ���� �Ÿ�)
	// theta : ����� ����(����, �Ű�����)�� �ػ�, �� ȸ�� ����. (��� ���⿡�� ������ �����Ϸ��� PI / 180 �� ����ϸ� �ȴ�.)
	// threshold : ���� ��ȯ�� �׷������� ������ �����ϱ� ���� �ּ� ���� ��

	int iThreshold1_canny = 0;
	int iThreshold2_canny = 300;
	int iThershold_lines = 50; // ���� ��ȯ�� �׷������� ������ �����ϱ� ���� �ּ� ���� �� (���� Ŭ���� ��Ȯ������, �������� ��ȣ����)

	Mat dstImage(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, dstImage, COLOR_GRAY2BGR);

	// ���� ���� (���� ����)
	Mat edges;
	Canny(dstImage, edges, iThreshold1_canny, iThreshold2_canny);

	// Mat lines;
	vector<Vec2f> lines;
	HoughLines(edges, lines, 1, CV_PI / 180.0, iThershold_lines);
	//HoughLines(srcImage, lines, 1, CV_PI / 180.0, iThershold_lines);
	cout << "lines.size()=" << lines.size() << endl;	// ����� ������ ����

	// ������ ���� �׸���
	Mat edges_lane;
	threshold(edges, edges_lane, iThreshold1_canny, iThreshold2_canny, THRESH_MASK);

	Vec2f params;
	float fRho = 0, fTheta = 0;
	float c = 0, s = 0;
	float fX0 = 0, fY0 = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		params = lines[i];
		fRho = params[0];
		fTheta = params[1];

		c = cos(fTheta);
		s = sin(fTheta);
		fX0 = fRho * c;
		fY0 = fRho * s;

		Point pt1, pt2;
		pt1.x = cvRound(fX0 + 1000 * (-s));
		pt1.y = cvRound(fY0 + 1000 * (c));
		pt2.x = cvRound(fX0 - 1000 * (-s));
		pt2.y = cvRound(fY0 - 1000 * (c));
		line(dstImage, pt1, pt2, Scalar(0, 0, 255), 2);
		line(edges_lane, pt1, pt2, Scalar::all(255), 2);

		printf("line distance : %d %d\n", pt1.x - pt2.x, pt1.y - pt2.y);

	}


	// �� ����
	double dThreshold1_dp = 1; // dp = 1 >> �Է� �̹����� ���� �ػ�, dp = 2 >> �Է� �̹����� ���� �ػ�
	double dThreshold2_mindist = 1;	// ������ ���� �ּҰŸ�
	double dParam2 = 30; // ���� �迭���� �������� ���� �Ӱ谪

	//Mat circles;
	vector <Vec3f> circles;
	HoughCircles(srcImage, circles, HOUGH_GRADIENT, dThreshold1_dp, dThreshold2_mindist, iThreshold2_canny, dParam2);
	cout << "circles.size()=" << circles.size() << endl;

	Mat dstImageCircle(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, dstImageCircle, COLOR_GRAY2BGR);

	Vec3f params_circle;
	int cx = 0, cy = 0, r = 0;
	Point pCenter;

	for (int i = 0; i < circles.size(); i++)
	{
		params_circle = circles[i];
		cx = cvRound(params_circle[0]);
		cy = cvRound(params_circle[1]);
		r = cvRound(params_circle[2]);
		printf("circles[%2d]:(cx, cy)=(%d, %d), r=%d\n", i, cx, cy, r);

		pCenter.x = cx;
		pCenter.y = cy;

		// �� �׸���
		circle(dstImageCircle, pCenter, r, Scalar(0, 0, 255), 2);
	}


	// �̹��� ���
	//imshow("org", srcImage);
	imshow("edges", edges);
	//imshow("dstImage", dstImage);
	imshow("laneImage", edges_lane);
	//imshow("Circlemage", dstImageCircle);


	// �ƹ�Ű�� ������ ������ ���
	waitKey();

	return 0;
}

// ȭ����� �߽ɰ� ����� ���� �߽� ������ X, Y �ȼ��Ÿ��� �����ش�.
// ��(-) ~ ��(+), ��(-) ~ ��(+)
// pDistX, pDistY : ���� ������ �ȼ��Ÿ�����
// imagesrc : �̹��� ���
//double dbThreshold_dp = 1; // dp = 1 >> �Է� �̹����� ���� �ػ�, dp = 2 >> �Է� �̹����� ���� �ػ�
//double dbThreshold_mindist = 2;	// ������ ���� �ּҰŸ�
//double dbThreshold_canny = 500;
//double dbThreshold_detection = 50; // ���� �迭���� �������� ���� �Ӱ谪
// �� �Լ��� ����� ���� 1���� ��쿡�� �����۵��Ѵ�.
int GetDistFromCircle
(
	unsigned char* imagesrc,
	int iImageWidth,
	int iImageHeight,
	int* pDistX,
	int* pDistY,
	double dbDp,
	double dbMindist,
	double dbThreshold_canny,
	double dbThreshold_detection,
	double dbminRadius,
	double dbmaxRadius,
	int iIndex
)
{
	// �̹����ҽ� �ҷ��´�.
	//Mat srcImage = imread((const char*)imagesrc, IMREAD_GRAYSCALE);
	Mat srcImage = Mat(iImageHeight, iImageWidth, CV_8UC1, imagesrc);
	if (srcImage.empty()) return -1;

	// �̹��� Blur ó�� (����������)
	Mat srcImage_blurred;
	GaussianBlur(srcImage, srcImage_blurred, cv::Size(7, 7), 1.5, 1.5);

	// �̹��� �߽� ���Ѵ�.
	cv::Point PCenterOfScreen;
	PCenterOfScreen.x = (int)(srcImage.cols / 2);
	PCenterOfScreen.y = (int)(srcImage.rows / 2);
	//printf("�̹��� �߽� x,y : %d, %d\n", PCenterOfScreen.x, PCenterOfScreen.y);

	// �̹����ҽ� �÷�ȭ
	Mat dstImageCircle;	
	dstImageCircle = Mat(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, dstImageCircle, COLOR_GRAY2BGR);
	line(dstImageCircle, PCenterOfScreen, PCenterOfScreen, Scalar::all(0), 2);
	


	// ����� ���� �ּ� ������, �ִ� ������ mm to pixel
	double dbDPI = (double)CAMERA_DPI;	// Huaray ī�޶��� �ػ� = 2540 DPI
	int iminRadiusPixel = (int)((dbminRadius * dbDPI) / 25.4);
	int imaxRadiusPixel = (int)((dbmaxRadius * dbDPI) / 25.4);

	//printf("%d, %d\n", iminRadiusPixel, imaxRadiusPixel);

	// �� ����
	//Mat circles;
	vector <Vec3f> circles;

	// ���� ����� ������ canny �� �����鼭 �ݺ�����
	double local_dbThreshold_canny = dbThreshold_canny;
	int iboundary = 50;
	while (local_dbThreshold_canny > iboundary)
	{
#ifdef HOUGH
		HoughCircles(srcImage_blurred, circles, HOUGH_GRADIENT, dbDp, dbMindist, local_dbThreshold_canny, dbThreshold_detection, iminRadiusPixel, imaxRadiusPixel);
#endif
#ifdef HOUGH_ALT
		HoughCircles(srcImage_blurred, circles, HOUGH_GRADIENT_ALT, dbDp, dbMindist, local_dbThreshold_canny, dbThreshold_detection, iminRadiusPixel, imaxRadiusPixel);
#endif
		if (circles.size() > 0) break;
		local_dbThreshold_canny -= 10;
	}

	if (local_dbThreshold_canny <= iboundary) return -1;

	// ���� ����� ������ detection �� �����鼭 �ݺ�����
	//double local_dbThreshold_detection = dbThreshold_detection;
	//while (local_dbThreshold_detection > 0)
	//{
	//	HoughCircles(srcImage_blurred, circles, HOUGH_GRADIENT, dbDp, dbMindist, dbThreshold_canny, local_dbThreshold_detection, iminRadiusPixel, imaxRadiusPixel);
	//	if (circles.size() > 0)
	//		break;
	//	local_dbThreshold_detection -= 5;
	//}

	int iCircleSize = circles.size();
	//cout << "circles.size()=" << iCircleSize << endl;

	Vec3f params_circle;
	cv::Point PCenterOfCircle;
	int r = 0;

	for (int i = 0; i < iCircleSize; i++)
	{
		params_circle = circles[i];
		PCenterOfCircle.x = cvRound(params_circle[0]);
		PCenterOfCircle.y = cvRound(params_circle[1]);
		r = cvRound(params_circle[2]);
		//printf("circles[%2d]:(cx, cy)=(%d, %d), r = %d\n", i, PCenterOfCircle.x, PCenterOfCircle.y, r);

		// �� �׸��� (�׽�Ʈ��)
	
		circle(dstImageCircle, PCenterOfCircle, r, Scalar(0, 0, 255), 2);
		line(dstImageCircle, PCenterOfCircle, PCenterOfCircle, Scalar(0, 0, 255), 2);
		
		// ȭ����� �߽����κ��� ����� ���� �߽ɻ����� �Ÿ� ���ϱ� (X, Y) �ȼ�����
		// ����� �� �߽� - ȭ����� �߽�
		(*pDistX) = (int)(PCenterOfCircle.x - PCenterOfScreen.x);
		(*pDistY) = (int)(PCenterOfCircle.y - PCenterOfScreen.y);

		printf("Distance from Circle : %d, %d\n", (*pDistX), (*pDistY));
		//printf("Distance from Circle : %d, %d\n", PCenterOfCircle.x, PCenterOfCircle.y);
	}

	// �̹��� ��� (�׽�Ʈ��)
	// ȭ�� �߽ɰ� ���� �߽ɻ��� ������ �ߴ´�. (�׽�Ʈ��)
	line(dstImageCircle, PCenterOfScreen, PCenterOfCircle, Scalar::all(0), 2);

	//imshow("orgsrc", srcImage);
	//imshow("GetCenterOfCircle", dstImageCircle);
	char cArrFileName[100] = { 0, };
	sprintf(cArrFileName, "test1_R3\\test%d.jpg", iIndex);
	imwrite(cArrFileName, dstImageCircle);

	// �ƹ�Ű�� ������ ������ ���
	waitKey();
	

	return 0;
}