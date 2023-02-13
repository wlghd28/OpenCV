//opencv header file include
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"
#include <windows.h>

#define CAMERA_DPI 2540
#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 1024

//#define HOUGH
//#define HOUGH_ALT
#define CONTOUR

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
	double dbThreshold_max_canny,
	double dbThreshold_min_canny,
	double dbThreshold_detection,
	double dbminRadius,
	double dbmaxRadius,
	int iIndex
);

// pDistX, pDistY : ���� ������ �ȼ��Ÿ�����
// imagesrc : �̹��� ���
// iThresholdBlockSize : Threshold ��� ������ (������ Ȧ������ ��)
// dbThresholdOffset : �Ӱ谪�� ���ϱ� ���� ��� �� ������տ��� �� ��
// dbMinArea : ����� ��� ��ǥ�� �ٻ�ȭ�� ��ǥ�� ���� �Ÿ��� �ּ� ��
// dbMinRatio : ??
int GetDistFromContours
(
	unsigned char* imagesrc,
	int iImageWidth,
	int iImageHeight,
	int* pDistX,
	int* pDistY,
	int iThresholdBlockSize,
	double dbThresholdOffset,
	double dbMinArea,
	double dbMinRatio,
	int iIndex
);




//project main function
int main(int argc, char** argv) {

	// �׽�Ʈ�� �Լ�
	//Test();

	int iDistX = 0;
	int iDistY = 0;

	char cbuf[256] = { 0, };
	Mat srcImage;
	//srcImage = imread((const char*)"test1_R2.jpg", IMREAD_GRAYSCALE);
	for (int i = 0; i < 8; i++)
	{
		sprintf(cbuf, "test%d_R.bmp", i + 1);
		srcImage = imread((const char*)cbuf, IMREAD_GRAYSCALE);
#ifdef HOUGH
		GetDistFromCircle((unsigned char*)srcImage.ptr(), 1280, 1024, &iDistX, &iDistY, 1, 9999, 150, 0, 40, 1.3, 1.7, i + 1);
#endif
#ifdef HOUGH_ALT
		GetDistFromCircle((unsigned char*)srcImage.ptr(), 1280, 1024, &iDistX, &iDistY, 1.5, 5, 300, 0.9, 1, 3, i);
#endif
#ifdef CONTOUR
		GetDistFromContours((unsigned char*)srcImage.ptr(), 1280, 1024, &iDistX, &iDistY, 127, 5, 30000, 0, i + 1);
#endif
	}
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
	double dbThreshold_max_canny,
	double dbThreshold_min_canny,
	double dbThreshold_detection,
	double dbminRadius,
	double dbmaxRadius,
	int iIndex
)
{
	int starttime = 0;
	int endtime = 0;

	starttime = GetTickCount64();

	// �̹����ҽ� �ҷ��´�.
	//Mat srcImage = imread((const char*)imagesrc, IMREAD_GRAYSCALE);
	Mat srcImage = Mat(iImageHeight, iImageWidth, CV_8UC1, imagesrc);
	if (srcImage.empty()) return -1;

	int iCutStartX = 100;
	int iCutStartY = 100;

	srcImage = srcImage(cv::Rect(iCutStartX, iCutStartY, CAMERA_WIDTH - iCutStartX * 2, CAMERA_HEIGHT  - iCutStartY * 2));

	// �̹��� Blur ó��
	Mat srcImage_blurred;
	//GaussianBlur(srcImage, srcImage_blurred, cv::Size(3, 3), 0);
	GaussianBlur(srcImage, srcImage_blurred, cv::Size(7, 7), 1.5, 1.5, BORDER_DEFAULT);
	//bilateralFilter(srcImage, srcImage_blurred, -1, 50, 50, BORDER_DEFAULT);

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
	double local_dbThreshold_canny = dbThreshold_max_canny;
	double local_dbThershold_min_canny = dbThreshold_min_canny;
	while (local_dbThreshold_canny > local_dbThershold_min_canny)
	{
#ifdef HOUGH
		HoughCircles(srcImage_blurred, circles, HOUGH_GRADIENT, dbDp, dbMindist, local_dbThreshold_canny, dbThreshold_detection, iminRadiusPixel, imaxRadiusPixel);
#endif
#ifdef HOUGH_ALT
		HoughCircles(srcImage_blurred, circles, HOUGH_GRADIENT_ALT, dbDp, dbMindist, local_dbThreshold_canny, dbThreshold_detection, iminRadiusPixel, imaxRadiusPixel);
#endif
		if (circles.size() > 0) break;
		local_dbThreshold_canny -= 10;
		//local_dbThreshold_canny--;
		//local_dbThreshold_canny -= 0.1;
	}

	endtime = GetTickCount64();

	printf("time : %d\n", endtime - starttime);

	if (local_dbThreshold_canny <= local_dbThershold_min_canny) return -1;

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
	sprintf(cArrFileName, "test%d_L\\test%d.jpg", iIndex, iIndex);
	//sprintf(cArrFileName, "test1_R3\\test%d.jpg", iIndex);
	imwrite(cArrFileName, dstImageCircle);

	// �ƹ�Ű�� ������ ������ ���
	waitKey();
	

	return 0;
}



int GetDistFromContours
(
	unsigned char* imagesrc,
	int iImageWidth,
	int iImageHeight,
	int* pDistX,
	int* pDistY,
	int iThresholdBlockSize,
	double dbThresholdOffset,
	double dbMinArea,
	double dbMinRatio,
	int iIndex
)
{
	if (iThresholdBlockSize % 2 == 0)
		iThresholdBlockSize--;
	
	if (iThresholdBlockSize < 3)
		iThresholdBlockSize = 3;


	int starttime = 0;
	int endtime = 0;

	starttime = GetTickCount64();

	// �̹����ҽ� �ҷ��´�.
	//Mat srcImage = imread((const char*)imagesrc, IMREAD_GRAYSCALE);
	Mat srcImage = Mat(iImageHeight, iImageWidth, CV_8UC1, imagesrc);
	if (srcImage.empty()) return -1;

	int iCutStartX = 100;
	int iCutStartY = 100;

	srcImage = srcImage(cv::Rect(iCutStartX, iCutStartY, CAMERA_WIDTH - iCutStartX * 2, CAMERA_HEIGHT - iCutStartY * 2));

	// ��� �̹��� Blur ó��
	Mat srcImage_blurred;
	//GaussianBlur(srcImage, srcImage_blurred, cv::Size(3, 3), 0);
	//GaussianBlur(srcImage, srcImage_blurred, cv::Size(7, 7), 1.5, 1.5, BORDER_DEFAULT);
	bilateralFilter(srcImage, srcImage_blurred, -1, 10, 5, BORDER_DEFAULT);

	// �̹��� �߽� ���Ѵ�.
	cv::Point PCenterOfScreen;
	PCenterOfScreen.x = (int)(srcImage.cols / 2);
	PCenterOfScreen.y = (int)(srcImage.rows / 2);
	//printf("�̹��� �߽� x,y : %d, %d\n", PCenterOfScreen.x, PCenterOfScreen.y);

	// �̹����ҽ� �÷�ȭ (�׽�Ʈ ��)
	Mat srcImage_color;
	srcImage_color = Mat(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, srcImage_color, COLOR_GRAY2BGR);
	line(srcImage_color, PCenterOfScreen, PCenterOfScreen, Scalar::all(0), 2);


	// ������ Threshold ����
	//threshold(srcImage_blurred, srcImage_blurred, 169, 255, THRESH_BINARY_INV);
	adaptiveThreshold(srcImage_blurred, srcImage_blurred, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, iThresholdBlockSize, dbThresholdOffset);

	// ��Ȱȭ
	equalizeHist(srcImage_blurred, srcImage_blurred);

	//imshow("threshold", srcImage_blurred);

	// ��� ã�� (findContours) 
	vector <vector<Point>> contours;
	findContours(srcImage_blurred, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE/*CHAIN_APPROX_SIMPLE*/, Point(0, 0));

	// ��� ���� ����
	if (contours.size() <= 0) return -1;

	// ã�� ��� �׸���
	//drawContours(srcImage_color, contours, -1, (255, 0, 0), 2);


	 /// Approximate contours to polygons + get bounding rects and circles
	bool bCheckDetection = false;
	int ivtc = 0;
	double dblen = 0;
	double dbarea = 0;
	double dbratio = 0;
	double dbmin_area = (double)dbMinArea;
	double dbmin_ratio = (double)dbMinRatio * 0.1;
	//Rect rc;
	Point2f center;
	float fradius;
	vector<Point> approx;
	for (vector<Point> &pts : contours)
	{
		approxPolyDP(pts, approx, arcLength(pts, true) * 0.02, true);

		ivtc = (int)approx.size();
		if (4 < ivtc)
		{
			dblen = arcLength(pts, true);
			dbarea = contourArea(pts);
			dbratio = 4.0 * CV_PI * dbarea / (dblen * dblen);
			if (dbmin_ratio < dbratio && dbmin_area < dbarea)
			{
				//rc = boundingRect(pts);
				//rectangle(srcImage_color, rc, (0, 0, 255), 1);
				minEnclosingCircle(pts, center, fradius);
				circle(srcImage_color, center, (int)fradius, (0, 0, 255), 2, 8, 0);

				// ����� �� �߽� - ȭ����� �߽�
				(*pDistX) = (int)(center.x - PCenterOfScreen.x);
				(*pDistY) = (int)(center.y - PCenterOfScreen.y);

				printf("Distance from Circle : %d, %d\n", (*pDistX), (*pDistY));
				bCheckDetection = true;
			}
		}
	}

	endtime = GetTickCount64();
	printf("time : %d\n", endtime - starttime);

	if (!bCheckDetection) return -1;


	// �̹��� ��� (�׽�Ʈ��)
	// ȭ�� �߽ɰ� ���� �߽ɻ��� ������ �ߴ´�. (�׽�Ʈ��)
	line(srcImage_color, PCenterOfScreen, center, Scalar::all(0), 2);

	char cArrFileName[100] = { 0, };
	sprintf(cArrFileName, "test%d_R\\contours%d.jpg", iIndex, iIndex);
	//sprintf(cArrFileName, "test1_R2\\contours%d.jpg", iIndex);
	imwrite(cArrFileName, srcImage_color);
	imshow("test", srcImage_color);

	// �ƹ�Ű�� ������ ������ ���
	waitKey();


	return 0;
}