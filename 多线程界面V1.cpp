#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include<opencv2/opencv.hpp>
#include"putTextZh\putText.h"
using namespace std;
using namespace cv;
int key = 0;//捕获的键盘输入
int Threshold = 120; // 二值化的阈值
int Mouse_x = 0;//鼠标当前位置xy
int Mouse_y = 0;
int MouseState = -1;//鼠标状态。

const string  MainWindow = "白酒检测系统";
const string  Tracker = "二值化阈值";
int height = 480, width = 640;
//颜色表 https://blog.csdn.net/kc58236582/article/details/50563958 //  注意Scalar是 BGR顺序
Scalar yellow = Scalar(0, 255, 255);
Scalar red = Scalar(0, 0, 255);
Scalar white = Scalar(255, 255, 255);
Scalar black = Scalar(0, 0, 0);
Scalar green = Scalar(0, 255, 0);


DWORD WINAPI ThreadFunction1(LPVOID lpParameter);
DWORD WINAPI ThreadFunction2(LPVOID lpParameter);
Mat Interface(Mat Input);//界面所在
void on_mouse(int event, int x, int y, int, void *);  //获取鼠标位置
void on_TrackerbarBW(int threshold, void *){}//条码作用

Mat ImageT1In = Mat::zeros(height, width, CV_8UC3);//线程1的输入图像
Mat ImageT1Out = Mat::zeros(height, width, CV_8UC3);//线程1的输出图像
Mat ImageT2Out = Mat::zeros(height, width, CV_8UC3);//线程2的输出图像
Mat ImageT2In = Mat::zeros(height, width, CV_8UC3);//线程1的输入图像
Mat ImageToshow = Mat::zeros(height * 2, width * 2.5, CV_8UC3);//被显示的图像

int main()
{
	HANDLE handle1, handle2;
	handle1 = CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
	handle2 = CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
	if (NULL == handle1)
	{
		cout << "创建线程1 失败 !" << endl;
		return -1;
	}
	if (NULL == handle2)
	{
		cout << "创建线程2 失败 !" << endl;
		return -1;
	}

	CloseHandle(handle1);
	CloseHandle(handle2);//创建完之后关闭线程
	cout << "主线程运行 !" << endl;
	VideoCapture cap;
	cap.open(0); //打开视频,以上两句等价于VideoCapture cap("E://2.avi");

	if (!cap.isOpened())//如果视频不能正常打开则返回
		return -1;
	Mat frame;
	namedWindow(MainWindow, CV_WINDOW_NORMAL);
	cv::setWindowProperty(MainWindow, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);//主界面设置全屏
	cv::setMouseCallback(MainWindow, on_mouse);//鼠标回调函数
	cv::createTrackbar(Tracker, MainWindow, &Threshold, 255, on_TrackerbarBW);//控制条barname, winname, &sidlervalue, g_max, ontracker
																		  //on_TrackerbarB

	while (1)
	{	//cout << "主线程 ！" << endl;
		
		ImageToshow = Mat::zeros(height * 2, width * 2.5, CV_8UC3);//被显示的图像
		cap >> frame;//等价于cap.read(frame);
		frame.copyTo(ImageT1In);
		frame.copyTo(ImageT2In);
		if (frame.empty())//如果某帧为空则退出循环
			break;
		//int threshold = cv::getTrackbarPos("Track1", "img");//tracker bar	
		Mat gray;
		cvtColor(ImageT2In, gray, COLOR_BGR2GRAY);
		cv::Mat BW = gray < Threshold;		
		cvtColor(BW, BW, COLOR_GRAY2BGR);
		 
		Rect rectT1, rectT2, rectT3,  rectT4;
		rectT1.x = 0;		rectT1.y = 0;		rectT1.width = width;		rectT1.height = height;
		rectT2.x = 0;		rectT2.y = height;		rectT2.width = width;		rectT2.height = height;
		rectT3.x = width;		rectT3.y = 0;		rectT3.width = width;		rectT3.height = height;
		rectT4.x = width;		rectT4.y = height;		rectT4.width = width;		rectT4.height = height;
		Mat Region1 = ImageToshow(rectT1); Mat Region2 = ImageToshow(rectT2); Mat Region3 = ImageToshow(rectT3); Mat Region4 = ImageToshow(rectT4);
		ImageT1Out.copyTo(Region1);
		ImageT2Out.copyTo(Region2);
		frame.copyTo(Region3);
		BW.copyTo(Region4);
	 

		ImageToshow = Interface(ImageToshow);//调用显示		
		imshow(MainWindow, ImageToshow);
		key = (char)cv::waitKey(1);
		if (MouseState > -1)
		{
			key = MouseState;
			MouseState = -1;//默认鼠标状态-1
		}			
		switch (key)
		{
		case 'q':
		case 'Q':
		case 27: //escape key 按下q或者esc键，退出
			return false;	
		case '+':
		case '=':
			Threshold += 10;//按下加号键
			break;
		case'-':
		case'_':
			Threshold -= 10;//按下减号键
			break;
		}
		
	}
	cap.release();//释放资源 
	system("PAUSE");
	return 0;
}
DWORD WINAPI ThreadFunction1(LPVOID lpParameter)
{
	Mat grayImage, edge;
	cout << "线程1 开始运行 ！" << endl;
	while (1)
	{
		cvtColor(ImageT1In, grayImage, COLOR_BGR2GRAY);
		//先使用3*3内核来降噪
		blur(grayImage, grayImage, Size(3, 3));
		//运行canny算子
		Canny(grayImage, edge, 3, 9, 3);
		//imshow("边缘提取效果", edge);

		cvtColor(edge * 255, ImageT1Out, COLOR_GRAY2BGR);
		//imshow("ImageT1Out", ImageT1Out);
		//waitKey(1);
		
		//Sleep(100);
	}
	return 0;
	 
}
DWORD WINAPI ThreadFunction2(LPVOID lpParameter)
{	cout << "线程2运行 ! " << endl;
	////线程2
	Mat grayImage, advBW;
	while (1)
	{
		cvtColor(ImageT2In, grayImage, COLOR_BGR2GRAY);
		adaptiveThreshold(grayImage, advBW, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 31, 5);//自适应二值化
		cvtColor(advBW, ImageT2Out, COLOR_GRAY2BGR);
		/*imshow("advBW", advBW);
		cv::waitKey(100);*/
		//cout << "Thread 02 is working ！" << endl;
		//Sleep(200);
	}
	return 0;
}

Mat Interface(Mat Input) 
{
	//界面可以在这里画
	putTextZH(Input, "按Esc/点击此处退出", cv::Point(1300+20, 20), yellow, 24, "Times New Roman", false, false);
	//写中文
	//画矩形 //退出的范围
	rectangle(Input, Point(1300, 0), Point(width*2.5-10, 80), white,3, 1, 0);
	char st[200];	sprintf_s(st, sizeof(st), "鼠标位置x:%d, y:%d", Mouse_x, Mouse_y);		
	putTextZH(Input, st, cv::Point(Mouse_x, Mouse_y), yellow, 24, "Times New Roman", false, false);
	sprintf_s(st, sizeof(st), "二值化阈值: %d", Threshold);
	putTextZH(Input, st, cv::Point(1300 + 20, 100), yellow, 24, "Times New Roman", false, false);	 
	rectangle(Input, Point(1300, 80), Point(width*2.5 - 10, 160), white, 3, 1, 0);

	return Input;
}
void on_mouse(int event, int x, int y, int, void *)  //获取鼠标位置
{
	//if (event == EVENT_MOUSEMOVE)

	Mouse_x = x;
	Mouse_y = y;

	if (event == cv::EVENT_LBUTTONDOWN)
	{//如果鼠标左键		
		if (x > 1300 && x < width*2.5 &&y>0 && y < 80)
		{// 如果鼠标在这个范围内
			MouseState = 27;//退出
			 
		}
		 
	}
}
