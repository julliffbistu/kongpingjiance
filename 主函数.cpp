#include <iostream>
#include <thread>
#include <Windows.h>
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;
int height = 480, width = 640;
Mat ImageT1In= Mat::zeros(height, width, CV_8UC3);
Mat ImageT1Out = Mat::zeros(height, width, CV_8UC3);
Mat ImageT2Out = Mat::zeros(height, width, CV_8UC3);
Mat ImageT2In = Mat::zeros(height, width, CV_8UC3);
Mat ImageToshow = Mat::zeros(height*2, width*2, CV_8UC3);
void thread01()
{  //线程1
	Mat grayImage, edge;
	while (1)
	{
		cvtColor(ImageT1In, grayImage, COLOR_BGR2GRAY);
		//先使用3*3内核来降噪
		blur(grayImage, grayImage, Size(3, 3));
		//运行canny算子
		Canny(grayImage, edge, 3, 9, 3);
		//imshow("边缘提取效果", edge);
			 
		cvtColor(edge*255, ImageT1Out, COLOR_GRAY2BGR);
		//imshow("ImageT1Out", ImageT1Out);
		//waitKey(1);
		cout << "Thread 01 is working ！" << endl;
		//Sleep(100);
	}
}
void thread02()
{   ////线程2
	Mat grayImage, advBW;
	while(1)
	{
		cvtColor(ImageT2In, grayImage, COLOR_BGR2GRAY);
		adaptiveThreshold(grayImage, advBW, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 31, 5);//自适应二值化
		imshow("advBW", advBW);
		
		//for (int i = 1; i < 10000000000; i++)//假装这个需要很长时间
		//{
		//	int a = 10,b=10;
		//	int c = 1 + a + b;
		//	for (int j = 1; j < 10000000000; j++)
		//		c += 1;

		//}
		cvtColor(advBW, ImageT2Out, COLOR_GRAY2BGR);
		cv::waitKey(100);
		cout << "Thread 02 is working ！" << endl;
		//Sleep(200);
	}
}

int main()
{	//主函数里有个主线程
	VideoCapture cap;
	cap.open(0); //打开视频,以上两句等价于VideoCapture cap("E://2.avi");

	if (!cap.isOpened())//如果视频不能正常打开则返回
		return -1;
	Mat frame;
	thread task01(thread01);
	thread task02(thread02);
	task01.detach();
	task02.detach();
	while (1)
	{
		cap >> frame;//等价于cap.read(frame);
		frame.copyTo(ImageT1In);
		frame.copyTo(ImageT2In);
		if (frame.empty())//如果某帧为空则退出循环
			break;		
		Rect rectT1, rectT2, rectT3;
		rectT1.x = width;		rectT1.y = 0;		rectT1.width = width;		rectT1.height = height;
		rectT2.x = width;		rectT2.y = height;		rectT2.width = width;		rectT2.height = height;
		rectT3.x = 0;		rectT3.y = 0;		rectT3.width = width;		rectT3.height = height;
		Mat Region1=ImageToshow(rectT1) ; Mat Region2 = ImageToshow(rectT2); Mat Region3 = ImageToshow(rectT3);
		ImageT1Out.copyTo(Region1);		
		ImageT2Out.copyTo(Region2);
		frame.copyTo(Region3);		 
		imshow("ImageToshow", ImageToshow);
		waitKey(20);//每帧延时20毫秒
		cout << "主线程 ！" << endl;
	}
	cap.release();//释放资源
	return 0;
}

