#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include<opencv2/opencv.hpp>
#include"putTextZh\putText.h"
using namespace std;
using namespace cv;
int key = 0;//����ļ�������
int Threshold = 120; // ��ֵ������ֵ
int Mouse_x = 0;//��굱ǰλ��xy
int Mouse_y = 0;
int MouseState = -1;//���״̬��

const string  MainWindow = "�׾Ƽ��ϵͳ";
const string  Tracker = "��ֵ����ֵ";
int height = 480, width = 640;
//��ɫ�� https://blog.csdn.net/kc58236582/article/details/50563958 //  ע��Scalar�� BGR˳��
Scalar yellow = Scalar(0, 255, 255);
Scalar red = Scalar(0, 0, 255);
Scalar white = Scalar(255, 255, 255);
Scalar black = Scalar(0, 0, 0);
Scalar green = Scalar(0, 255, 0);


DWORD WINAPI ThreadFunction1(LPVOID lpParameter);
DWORD WINAPI ThreadFunction2(LPVOID lpParameter);
Mat Interface(Mat Input);//��������
void on_mouse(int event, int x, int y, int, void *);  //��ȡ���λ��
void on_TrackerbarBW(int threshold, void *){}//��������

Mat ImageT1In = Mat::zeros(height, width, CV_8UC3);//�߳�1������ͼ��
Mat ImageT1Out = Mat::zeros(height, width, CV_8UC3);//�߳�1�����ͼ��
Mat ImageT2Out = Mat::zeros(height, width, CV_8UC3);//�߳�2�����ͼ��
Mat ImageT2In = Mat::zeros(height, width, CV_8UC3);//�߳�1������ͼ��
Mat ImageToshow = Mat::zeros(height * 2, width * 2.5, CV_8UC3);//����ʾ��ͼ��

int main()
{
	HANDLE handle1, handle2;
	handle1 = CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
	handle2 = CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
	if (NULL == handle1)
	{
		cout << "�����߳�1 ʧ�� !" << endl;
		return -1;
	}
	if (NULL == handle2)
	{
		cout << "�����߳�2 ʧ�� !" << endl;
		return -1;
	}

	CloseHandle(handle1);
	CloseHandle(handle2);//������֮��ر��߳�
	cout << "���߳����� !" << endl;
	VideoCapture cap;
	cap.open(0); //����Ƶ,��������ȼ���VideoCapture cap("E://2.avi");

	if (!cap.isOpened())//�����Ƶ�����������򷵻�
		return -1;
	Mat frame;
	namedWindow(MainWindow, CV_WINDOW_NORMAL);
	cv::setWindowProperty(MainWindow, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);//����������ȫ��
	cv::setMouseCallback(MainWindow, on_mouse);//���ص�����
	cv::createTrackbar(Tracker, MainWindow, &Threshold, 255, on_TrackerbarBW);//������barname, winname, &sidlervalue, g_max, ontracker
																		  //on_TrackerbarB

	while (1)
	{	//cout << "���߳� ��" << endl;
		
		ImageToshow = Mat::zeros(height * 2, width * 2.5, CV_8UC3);//����ʾ��ͼ��
		cap >> frame;//�ȼ���cap.read(frame);
		frame.copyTo(ImageT1In);
		frame.copyTo(ImageT2In);
		if (frame.empty())//���ĳ֡Ϊ�����˳�ѭ��
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
	 

		ImageToshow = Interface(ImageToshow);//������ʾ		
		imshow(MainWindow, ImageToshow);
		key = (char)cv::waitKey(1);
		if (MouseState > -1)
		{
			key = MouseState;
			MouseState = -1;//Ĭ�����״̬-1
		}			
		switch (key)
		{
		case 'q':
		case 'Q':
		case 27: //escape key ����q����esc�����˳�
			return false;	
		case '+':
		case '=':
			Threshold += 10;//���¼Ӻż�
			break;
		case'-':
		case'_':
			Threshold -= 10;//���¼��ż�
			break;
		}
		
	}
	cap.release();//�ͷ���Դ 
	system("PAUSE");
	return 0;
}
DWORD WINAPI ThreadFunction1(LPVOID lpParameter)
{
	Mat grayImage, edge;
	cout << "�߳�1 ��ʼ���� ��" << endl;
	while (1)
	{
		cvtColor(ImageT1In, grayImage, COLOR_BGR2GRAY);
		//��ʹ��3*3�ں�������
		blur(grayImage, grayImage, Size(3, 3));
		//����canny����
		Canny(grayImage, edge, 3, 9, 3);
		//imshow("��Ե��ȡЧ��", edge);

		cvtColor(edge * 255, ImageT1Out, COLOR_GRAY2BGR);
		//imshow("ImageT1Out", ImageT1Out);
		//waitKey(1);
		
		//Sleep(100);
	}
	return 0;
	 
}
DWORD WINAPI ThreadFunction2(LPVOID lpParameter)
{	cout << "�߳�2���� ! " << endl;
	////�߳�2
	Mat grayImage, advBW;
	while (1)
	{
		cvtColor(ImageT2In, grayImage, COLOR_BGR2GRAY);
		adaptiveThreshold(grayImage, advBW, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 31, 5);//����Ӧ��ֵ��
		cvtColor(advBW, ImageT2Out, COLOR_GRAY2BGR);
		/*imshow("advBW", advBW);
		cv::waitKey(100);*/
		//cout << "Thread 02 is working ��" << endl;
		//Sleep(200);
	}
	return 0;
}

Mat Interface(Mat Input) 
{
	//������������ﻭ
	putTextZH(Input, "��Esc/����˴��˳�", cv::Point(1300+20, 20), yellow, 24, "Times New Roman", false, false);
	//д����
	//������ //�˳��ķ�Χ
	rectangle(Input, Point(1300, 0), Point(width*2.5-10, 80), white,3, 1, 0);
	char st[200];	sprintf_s(st, sizeof(st), "���λ��x:%d, y:%d", Mouse_x, Mouse_y);		
	putTextZH(Input, st, cv::Point(Mouse_x, Mouse_y), yellow, 24, "Times New Roman", false, false);
	sprintf_s(st, sizeof(st), "��ֵ����ֵ: %d", Threshold);
	putTextZH(Input, st, cv::Point(1300 + 20, 100), yellow, 24, "Times New Roman", false, false);	 
	rectangle(Input, Point(1300, 80), Point(width*2.5 - 10, 160), white, 3, 1, 0);

	return Input;
}
void on_mouse(int event, int x, int y, int, void *)  //��ȡ���λ��
{
	//if (event == EVENT_MOUSEMOVE)

	Mouse_x = x;
	Mouse_y = y;

	if (event == cv::EVENT_LBUTTONDOWN)
	{//���������		
		if (x > 1300 && x < width*2.5 &&y>0 && y < 80)
		{// �������������Χ��
			MouseState = 27;//�˳�
			 
		}
		 
	}
}
