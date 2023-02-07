//opencv header file include
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"

#define CAMERA_DPI 2540


#define HOUGH
//#define HOUGH_ALT

using namespace cv;
using namespace std;

// 함수 원형 선언
int Test();	// 테스트용


// pDistX, pDistY : 값이 구해질 픽셀거리변수
// imagesrc : 이미지 경로
// double dbThreshold_dp // dp = 1 >> 입력 이미지와 같은 해상도, dp = 2 >> 입력 이미지의 절반 해상도
// double dbThreshold_mindist // 검출할 원의 최소거리
// double dbThreshold_canny // 이미지를 흑백화 할 때 쓰이는 임계 값
// double dbThreshold_detection // 축적 배열에서 원검출을 위한 임계값 (값이 커질 수록 정확, 작을 수록 모호)
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

	// 테스트용 함수
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
	// 이미지소스 불러온다.
	Mat srcImage = imread("sample01.jpg", IMREAD_GRAYSCALE);
	if (srcImage.empty()) return -1;

	// 이미지 중심 구한다.
	//printf("이미지 중심 x,y : %d, %d\n", (int)(srcImage.cols / 2), (int)(srcImage.rows / 2));

	// HoughLines() 매개변수 설명
	// src : 입력할 이미지 변수, Edge detect 된 이미지를 입력해야 함
	// dst : 허프변환 직선 검출 정보를 저장할 Array
	// rho : 계산할 픽셀(매개 변수)의 해상도, 그냥 1을 사용하면 됨. (변환된 그래프에서, 선에서 원점까지의 수직 거리)
	// theta : 계산할 각도(라디안, 매개변수)의 해상도, 선 회전 각도. (모든 방향에서 직선을 검출하려면 PI / 180 을 사용하면 된다.)
	// threshold : 허프 변환된 그래프에서 라인을 검출하기 위한 최소 교차 수

	int iThreshold1_canny = 0;
	int iThreshold2_canny = 300;
	int iThershold_lines = 50; // 허프 변환된 그래프에서 라인을 검출하기 위한 최소 교차 수 (값이 클수록 정확해지고, 작을수록 모호해짐)

	Mat dstImage(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, dstImage, COLOR_GRAY2BGR);

	// 엣지 검출 (직선 검출)
	Mat edges;
	Canny(dstImage, edges, iThreshold1_canny, iThreshold2_canny);

	// Mat lines;
	vector<Vec2f> lines;
	HoughLines(edges, lines, 1, CV_PI / 180.0, iThershold_lines);
	//HoughLines(srcImage, lines, 1, CV_PI / 180.0, iThershold_lines);
	cout << "lines.size()=" << lines.size() << endl;	// 검출된 직선의 개수

	// 검출한 직선 그리기
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


	// 원 검출
	double dThreshold1_dp = 1; // dp = 1 >> 입력 이미지와 같은 해상도, dp = 2 >> 입력 이미지의 절반 해상도
	double dThreshold2_mindist = 1;	// 검출할 원의 최소거리
	double dParam2 = 30; // 축적 배열에서 원검출을 위한 임계값

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

		// 원 그리기
		circle(dstImageCircle, pCenter, r, Scalar(0, 0, 255), 2);
	}


	// 이미지 출력
	//imshow("org", srcImage);
	imshow("edges", edges);
	//imshow("dstImage", dstImage);
	imshow("laneImage", edges_lane);
	//imshow("Circlemage", dstImageCircle);


	// 아무키가 눌리기 전까지 대기
	waitKey();

	return 0;
}

// 화면상의 중심과 검출된 원의 중심 사이의 X, Y 픽셀거리를 구해준다.
// 상(-) ~ 하(+), 좌(-) ~ 우(+)
// pDistX, pDistY : 값이 구해질 픽셀거리변수
// imagesrc : 이미지 경로
//double dbThreshold_dp = 1; // dp = 1 >> 입력 이미지와 같은 해상도, dp = 2 >> 입력 이미지의 절반 해상도
//double dbThreshold_mindist = 2;	// 검출할 원의 최소거리
//double dbThreshold_canny = 500;
//double dbThreshold_detection = 50; // 축적 배열에서 원검출을 위한 임계값
// 이 함수는 검출된 원이 1개일 경우에만 정상작동한다.
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
	// 이미지소스 불러온다.
	//Mat srcImage = imread((const char*)imagesrc, IMREAD_GRAYSCALE);
	Mat srcImage = Mat(iImageHeight, iImageWidth, CV_8UC1, imagesrc);
	if (srcImage.empty()) return -1;

	// 이미지 Blur 처리 (노이즈제거)
	Mat srcImage_blurred;
	GaussianBlur(srcImage, srcImage_blurred, cv::Size(7, 7), 1.5, 1.5);

	// 이미지 중심 구한다.
	cv::Point PCenterOfScreen;
	PCenterOfScreen.x = (int)(srcImage.cols / 2);
	PCenterOfScreen.y = (int)(srcImage.rows / 2);
	//printf("이미지 중심 x,y : %d, %d\n", PCenterOfScreen.x, PCenterOfScreen.y);

	// 이미지소스 컬러화
	Mat dstImageCircle;	
	dstImageCircle = Mat(srcImage.size(), CV_8UC3);
	cvtColor(srcImage, dstImageCircle, COLOR_GRAY2BGR);
	line(dstImageCircle, PCenterOfScreen, PCenterOfScreen, Scalar::all(0), 2);
	


	// 검출될 원의 최소 반지름, 최대 반지름 mm to pixel
	double dbDPI = (double)CAMERA_DPI;	// Huaray 카메라의 해상도 = 2540 DPI
	int iminRadiusPixel = (int)((dbminRadius * dbDPI) / 25.4);
	int imaxRadiusPixel = (int)((dbmaxRadius * dbDPI) / 25.4);

	//printf("%d, %d\n", iminRadiusPixel, imaxRadiusPixel);

	// 원 검출
	//Mat circles;
	vector <Vec3f> circles;

	// 원이 검출될 때까지 canny 값 내리면서 반복수행
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

	// 원이 검출될 때까지 detection 값 내리면서 반복수행
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

		// 원 그리기 (테스트용)
	
		circle(dstImageCircle, PCenterOfCircle, r, Scalar(0, 0, 255), 2);
		line(dstImageCircle, PCenterOfCircle, PCenterOfCircle, Scalar(0, 0, 255), 2);
		
		// 화면상의 중심으로부터 검출된 원의 중심사이의 거리 구하기 (X, Y) 픽셀단위
		// 검출된 원 중심 - 화면상의 중심
		(*pDistX) = (int)(PCenterOfCircle.x - PCenterOfScreen.x);
		(*pDistY) = (int)(PCenterOfCircle.y - PCenterOfScreen.y);

		printf("Distance from Circle : %d, %d\n", (*pDistX), (*pDistY));
		//printf("Distance from Circle : %d, %d\n", PCenterOfCircle.x, PCenterOfCircle.y);
	}

	// 이미지 출력 (테스트용)
	// 화면 중심과 원의 중심사이 직선을 긋는다. (테스트용)
	line(dstImageCircle, PCenterOfScreen, PCenterOfCircle, Scalar::all(0), 2);

	//imshow("orgsrc", srcImage);
	//imshow("GetCenterOfCircle", dstImageCircle);
	char cArrFileName[100] = { 0, };
	sprintf(cArrFileName, "test1_R3\\test%d.jpg", iIndex);
	imwrite(cArrFileName, dstImageCircle);

	// 아무키가 눌리기 전까지 대기
	waitKey();
	

	return 0;
}