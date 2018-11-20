#include "FindCircles.h"
#include"opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

void main()
{
    Mat img = imread("quanlunkuo.jpg");//原图
	//imshow("yuantu",img);
    Mat cvtColor_output;//灰度图
    cvtColor(img, cvtColor_output, CV_BGR2GRAY);
    Mat threshold_output;//二值图
    threshold(cvtColor_output, threshold_output, 180, 255, CV_THRESH_BINARY);
	imshow("二值化结果",threshold_output);
    Mat Canny_output;
	//Mat dstImage = Mat::zeros(threshold_output.rows,threshold_output.cols,CV_8UC3);
	//threshold_output = threshold_output > 100;
	//imshow("【取阈值后的原始图】",threshold_output);
	//---------------------------------------------------定义层次结构----------------------------------------------------------
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//---------------------------------------------------查找轮廓--------------------------------------------------------------
	findContours(threshold_output,contours,hierarchy,CV_RETR_EXTERNAL,CHAIN_APPROX_NONE);
	int index = 0;
	Mat imageContours1=Mat::zeros(threshold_output.size(),CV_8UC1); 
	
	for ( ; index >= 0;index = hierarchy[index][0])
	{
		//Scalar color(255,255,255);
		Scalar color(rand()&255,rand()&255,rand()&255);//随机颜色
		drawContours(imageContours1,contours,index,Scalar(255),1,8,hierarchy); 
		 Point2f center; float radius; 
		minEnclosingCircle(contours[index],center,radius);
		if(radius>300)
		{
			circle(imageContours1,center,radius,Scalar(255)); 
	
		imshow("【轮廓图】",imageContours1);
		cout<<"yuanxin:"<<center<<"cvcccccccccc"<<center.y<<endl;

		cout<<"yuanxinqqqqq:"<<radius<<endl;
		}
	}
	/*int minr = 200;
    int maxr = 400;
    vector<Point3f> circles = FindCircles(threshold_output, 500, 100, 100, 240);//检测结果
    for (int i = 0; i < circles.size(); i++)
    {
        circle(img, Point(circles[i].x, circles[i].y), circles[i].z, Scalar(0,0,255), 2, 8, 0);
    }
 
    imshow("检测结果",img);*/
    waitKey(0);
}
