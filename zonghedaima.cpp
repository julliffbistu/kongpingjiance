
#include"opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#define PI 3.1415926
using namespace cv;
using namespace std;

int main()
{
	double Time = (double)cvGetTickCount();
	Mat img = imread("C:\\Users\\zhulifu\\Desktop\\sanpai\\cegao.bmp");//ԭͼ
	Mat IMG=imread("C:\\Users\\zhulifu\\Desktop\\sanpai\\ceshi12.bmp");          ////,6,8��Ҫ��Ҫ����Ϊ3��ƿ�Ӱڷ������⡣������û�¡�
	if (!img.data)
	{
		printf("������û�и�ͼƬ\n");
		return false;
	}
	if (!IMG.data)
	{
		printf("������û�и�ͼƬ\n");
		return false;
	}
	
	int height_high = img.rows;
	int width_high = img.cols;
	//cout<<"��ƿ��ͼ��ĳߴ��С����Ϊ��:"<<height<<endl;
	//cout<<"��ƿ��ͼ��ĳߴ��С���Ϊ��:"<<width<<endl;
	Rect rect(577,460, 500,400);  //1,2λ������㣬���ĺ�λ�ǿ��
	img = img(rect);//
	//imshow("yuantu",img);
	Mat cvtColor_output;//�Ҷ�ͼ
	cvtColor(img, cvtColor_output, CV_BGR2GRAY);
	Mat threshold_output;//��ֵͼ
	threshold(cvtColor_output, threshold_output, 200, 255, CV_THRESH_BINARY);  //100�����޸ģ����ڶ�ֵ������
	//imshow("threshold_output",threshold_output);
	//Mat edges;
	//Canny(threshold_output,edges,1, 5,5);
//----------------------------------------׿��ɨ��-----------------------------------------------------------
	//imshow("thresh1old_output",edges);
	int n =0;
	//ƿ��ƿ�ڱ�Ե����
	for (int i = 0;i<threshold_output.rows;i++)
	{
		int acount=0;
		for (int j = 0;j<threshold_output.cols;j++)
		{
			if(threshold_output.at<uchar>(i,j) == 0) 
			{
				acount++;
			}

			if(acount>200)
			{
				cout<<"�������Ϊ"<<acount<<endl;
				cout<<"�к�Ϊ��"<<i<<endl;
				n++;
//----------------------------------------�ж�ƿ�Ӹߵ�------------------------------------------------------------------------------						
				int bottle_height_regularsize = 170;//�궨ƿ�ڸ߶����غ���ԭ�ߴ�  �߶�����������1mm
				int bottle_errorsize= 10; //1mm��ռ������ֵ        
				if (i<(bottle_height_regularsize-bottle_errorsize))
				{
					printf("This bottle is taller than the standard bottle\n");
					return 0;
				}
				else if(i>(bottle_height_regularsize+bottle_errorsize))
				{
					printf("This bottle is lower than the standard bottle\n");
					return 0;
				}
				else
				{
					printf("This bottle is a standard bottle\n");
				}				
			}
			if(n>0)
			{
				break;
			}
		}		
	}
	cout<<"**********************************************************************************\n"<<endl;
	cout<<"-------------------------------ƿ�Ӹߵ��жϽ���-----------------------------------\n"<<endl;
		//imshow("IMG",IMG);
		int height = IMG.rows;
		int width = IMG.cols;
		//cout<<"��ƿ��ͼ��ĳߴ��С����Ϊ��:"<<height<<endl;
		//cout<<"��ƿ��ͼ��ĳߴ��С���Ϊ��:"<<width<<endl;
		Rect rect2(350,150, 850,850);  //1,2λ������㣬���ĺ�λ�ǿ��
		Mat image_roi = IMG(rect2);
		//imshow("jieququyu",image_roi);
//-------------------------------------------------ȡ��Ŀ������-------------------------------------------------------------
		Point center;
		Mat dst_xiao = Mat::zeros(image_roi.size(), image_roi.type());
		Mat mask_xiao = Mat::zeros(image_roi.size(),CV_8U);
		Mat dst_da = Mat::zeros(image_roi.size(), image_roi.type());
		Mat mask_da = Mat::zeros(image_roi.size(),CV_8U);
		Point circleCenter(413, 472);                           //�ֶ��궨����Բ��
		int R1 =250,R2=295;										//�ֶ��궨Բ���뾶

		float area_loop = PI*(R2*R2-R1*R1);
		//cout<<"Բ�����ֵ����Ϊ:"<<area_loop<<endl;
		circle(mask_xiao, circleCenter, R1, Scalar(255),-1);  //Բ�İ뾶�ɵ��ڲ���  �Դ�һ���
		image_roi.copyTo(dst_xiao, mask_xiao);
		circle(mask_da, circleCenter, R2, Scalar(255),-1);//Բ�İ뾶�ɵ��ڲ���  ��Сһ���
		image_roi.copyTo(dst_da, mask_da);
		Mat dst = dst_da - dst_xiao;
		imshow("dst",dst);
		//----------------------------------------����ȡԲ��----------------------------------------------------------------
		int count=0;
		float bad_rate = 0;
		
		threshold( dst, dst, 200, 255, CV_THRESH_BINARY );
		imshow("erzhihua",dst);
		//-------------------------------------------------���������-------------------------------------------------------
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		Canny(dst, dst, 300, 900, 3);
		findContours(dst, contours, hierarchy, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
		for (int i = 0; i < (int)contours.size(); i++)
		{
			Scalar color(rand() & 255, rand() & 255, rand() & 255);
			drawContours(dst, contours, i, Scalar(255), 1, 8,hierarchy);
		}
		//imshow("��������ͼ��", dst);
		float endg_dConArea = 0;
		//�������������
		for (int i = 0; i < (int)contours.size(); i++)
		{
			double g_dConArea = contourArea(contours[i], true);
			//cout << "��������������㺯����������ĵ�" << i << "�����������Ϊ����" << g_dConArea << endl;
			//--------------------------------------ȷ�������С�����ƿ���������----------����ռ�������-------------------
			if(abs(g_dConArea)>10)   //15Ϊ�ɵ��۲������޸�������ռ�����С
			{
				endg_dConArea =endg_dConArea+abs(g_dConArea);
			}
		}
			cout<<"�������Ϊ:"<<endg_dConArea<<endl;
		if (endg_dConArea>200)
		{
			bad_rate = endg_dConArea*100/area_loop;
		}
			
			
			if (bad_rate!=0)
			{
				cout<<"This is a broken bottle!\n "<<"the broken rate is :"<<bad_rate<<"%"<<endl;
				Time = (double)cvGetTickCount() - Time ;
				printf( "run time = %gms\n", Time /(cvGetTickFrequency()*1000) );
				//return 0;
			}
			else
			{
				cout<<"This is a good bottle!\n"<<endl;
			}
		//-------------------------------------------------����Ƿ�����ƿ��-------------------------------------------------------------
		Size axes;
		Mat gray;
		cvtColor(image_roi,gray,CV_BGR2GRAY);      //��imageת���ɻҶȷŵ�gray��
		Mat threshold_out;
		threshold( gray, threshold_out, 170, 255, CV_THRESH_BINARY );    //��ֵ�����Ӱ����Բ��� 
		imshow("qqwqwq",threshold_out);
		//vector<vector<Point>> contours;
		//vector<Vec4i> hierarchy;
		float midel_radius = 0,large_radius = 0, small_radius = 0;
		findContours(threshold_out, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat cimage  = Mat::zeros(threshold_out.size(), CV_8UC3);
		for(size_t i = 0; i < contours.size(); i++)
		{
			//��ϵĵ�����Ϊ6
			size_t count = contours[i].size();
			if( count < 550 )   //ȥ����ԲС��һ��������Բ,�����
				continue;

			//��Բ���
			RotatedRect box = fitEllipse(contours[i]);
			center.x = cvRound(box.center.x);     /*ȷ����Բbox��ֵcenter��size��angle*/
			center.y = cvRound(box.center.y);

			axes.width = cvRound(box.size.width*0.5);
			axes.height = cvRound(box.size.height*0.5);
			box.angle = -box.angle;

			if(abs(axes.width-axes.height)<2)         //��϶��Բ���趨����������ȷ����Ҫ��Բ2������������Բ�ĳ����Ტ���
			{
				drawContours(cimage , contours, (int)i, Scalar::all(255), 1, 8);
				ellipse(image_roi , box, Scalar(0,0,255), 1, CV_AA);
				imshow("�����Բ���", image_roi );
				//printf("����λ�ã�x=%d,y=%d\n",axes.width,axes.height);
				midel_radius = (axes.width+axes.height)/2;
				cout<<"�м�뾶Ϊ ��"<<midel_radius<<endl;
				float A_area = PI*axes.width*axes.height;
				float A_length = 2*PI*axes.height+4*(abs(axes.width-axes.height));
				float  circular_degree = (4*PI*A_area)/(A_length*A_length);
				cout<<"Բ��Ϊ��"<<circular_degree<<endl;
				if(midel_radius<270)              //��������߶ȵ��ڵĲ�������Ҫ�궨huantujiugai
				{
					small_radius= midel_radius;
				}
				else
				{
					large_radius= midel_radius;
				}

				cout<<"-------------------------------------����������Բ����-----------------------------------------\n"<<endl;
			}
		}
		//cout<<"small is :"<<small_radius<<"\n"<<"large is :"<<large_radius<<"\n"<<endl;
		//------------------------------------------------�����⾶-------------------------------------------------------------
		float INNERstanderd_bottle_r = 10.5,OUTstanderd_bottle_r = 13.0;
		float UPDOWN_deflection = 0.2;

		float pixcel_area =  0.06; //�궨
		float INNERCircle_radius =small_radius*pixcel_area;
		float OUTCircle_radius =large_radius*pixcel_area;
		cout<<"��Բ�İ뾶Ϊ ��"<<INNERCircle_radius<<endl;
		cout<<"��Բ�İ뾶Ϊ ��"<<OUTCircle_radius<<endl;

		if(INNERCircle_radius>INNERstanderd_bottle_r+0.2 || INNERCircle_radius<INNERstanderd_bottle_r-0.2)
		{
			//cout<<"the bottle INNERCircle_radius is unregulr for INNERstanderd_bottle_r  "<<endl;
			//return 0;
		}
		if(OUTCircle_radius>OUTstanderd_bottle_r+0.2 || OUTCircle_radius<OUTstanderd_bottle_r-0.2)
		{
			//cout<<"the bottle OUTCircle_radius is unregulr for OUTstanderd_bottle_r"<<endl;
			//return 0;
		}
	
	waitKey(0);
	Time = (double)cvGetTickCount() - Time ;
	printf( "run time = %gms\n", Time /(cvGetTickFrequency()*1000) );
	return 0;
}