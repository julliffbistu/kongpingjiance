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
{  //�߳�1
	Mat grayImage, edge;
	while (1)
	{
		cvtColor(ImageT1In, grayImage, COLOR_BGR2GRAY);
		//��ʹ��3*3�ں�������
		blur(grayImage, grayImage, Size(3, 3));
		//����canny����
		Canny(grayImage, edge, 3, 9, 3);
		//imshow("��Ե��ȡЧ��", edge);
			 
		cvtColor(edge*255, ImageT1Out, COLOR_GRAY2BGR);
		//imshow("ImageT1Out", ImageT1Out);
		//waitKey(1);
		cout << "Thread 01 is working ��" << endl;
		//Sleep(100);
	}
}
void thread02()
{   ////�߳�2
	Mat grayImage, advBW;
	while(1)
	{
		cvtColor(ImageT2In, grayImage, COLOR_BGR2GRAY);
		adaptiveThreshold(grayImage, advBW, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 31, 5);//����Ӧ��ֵ��
		imshow("advBW", advBW);
		
		//for (int i = 1; i < 10000000000; i++)//��װ�����Ҫ�ܳ�ʱ��
		//{
		//	int a = 10,b=10;
		//	int c = 1 + a + b;
		//	for (int j = 1; j < 10000000000; j++)
		//		c += 1;

		//}
		cvtColor(advBW, ImageT2Out, COLOR_GRAY2BGR);
		cv::waitKey(100);
		cout << "Thread 02 is working ��" << endl;
		//Sleep(200);
	}
}

int main()
{	//���������и����߳�
	VideoCapture cap;
	cap.open(0); //����Ƶ,��������ȼ���VideoCapture cap("E://2.avi");

	if (!cap.isOpened())//�����Ƶ�����������򷵻�
		return -1;
	Mat frame;
	thread task01(thread01);
	thread task02(thread02);
	task01.detach();
	task02.detach();
	while (1)
	{
		cap >> frame;//�ȼ���cap.read(frame);
		frame.copyTo(ImageT1In);
		frame.copyTo(ImageT2In);
		if (frame.empty())//���ĳ֡Ϊ�����˳�ѭ��
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
		waitKey(20);//ÿ֡��ʱ20����
		cout << "���߳� ��" << endl;
	}
	cap.release();//�ͷ���Դ
	return 0;
}

