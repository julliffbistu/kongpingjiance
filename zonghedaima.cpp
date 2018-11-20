
#include"opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#define PI 3.1415926
using namespace cv;
using namespace std;

int main()
{
	double Time = (double)cvGetTickCount();
	Mat img = imread("C:\\Users\\zhulifu\\Desktop\\sanpai\\cegao.bmp");//原图
	Mat IMG=imread("C:\\Users\\zhulifu\\Desktop\\sanpai\\ceshi12.bmp");          ////,6,8需要需要限制为3，瓶子摆放有问题。摆正则没事。
	if (!img.data)
	{
		printf("请检查有没有该图片\n");
		return false;
	}
	if (!IMG.data)
	{
		printf("请检查有没有该图片\n");
		return false;
	}
	
	int height_high = img.rows;
	int width_high = img.cols;
	//cout<<"【瓶口图像的尺寸大小长度为】:"<<height<<endl;
	//cout<<"【瓶口图像的尺寸大小宽度为】:"<<width<<endl;
	Rect rect(577,460, 500,400);  //1,2位数是起点，三四号位是跨度
	img = img(rect);//
	//imshow("yuantu",img);
	Mat cvtColor_output;//灰度图
	cvtColor(img, cvtColor_output, CV_BGR2GRAY);
	Mat threshold_output;//二值图
	threshold(cvtColor_output, threshold_output, 200, 255, CV_THRESH_BINARY);  //100参数修改，调节二值化参数
	//imshow("threshold_output",threshold_output);
	//Mat edges;
	//Canny(threshold_output,edges,1, 5,5);
//----------------------------------------卓行扫描-----------------------------------------------------------
	//imshow("thresh1old_output",edges);
	int n =0;
	//瓶子瓶口边缘点数
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
				cout<<"点的数量为"<<acount<<endl;
				cout<<"行号为："<<i<<endl;
				n++;
//----------------------------------------判断瓶子高低------------------------------------------------------------------------------						
				int bottle_height_regularsize = 170;//标定瓶口高度像素和像原尺寸  高度误差不超于正负1mm
				int bottle_errorsize= 10; //1mm所占的像素值        
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
	cout<<"-------------------------------瓶子高低判断结束-----------------------------------\n"<<endl;
		//imshow("IMG",IMG);
		int height = IMG.rows;
		int width = IMG.cols;
		//cout<<"【瓶口图像的尺寸大小长度为】:"<<height<<endl;
		//cout<<"【瓶口图像的尺寸大小宽度为】:"<<width<<endl;
		Rect rect2(350,150, 850,850);  //1,2位数是起点，三四号位是跨度
		Mat image_roi = IMG(rect2);
		//imshow("jieququyu",image_roi);
//-------------------------------------------------取得目标区域-------------------------------------------------------------
		Point center;
		Mat dst_xiao = Mat::zeros(image_roi.size(), image_roi.type());
		Mat mask_xiao = Mat::zeros(image_roi.size(),CV_8U);
		Mat dst_da = Mat::zeros(image_roi.size(), image_roi.type());
		Mat mask_da = Mat::zeros(image_roi.size(),CV_8U);
		Point circleCenter(413, 472);                           //手动标定坐标圆心
		int R1 =250,R2=295;										//手动标定圆环半径

		float area_loop = PI*(R2*R2-R1*R1);
		//cout<<"圆环部分的面积为:"<<area_loop<<endl;
		circle(mask_xiao, circleCenter, R1, Scalar(255),-1);  //圆的半径可调节参数  略大一点好
		image_roi.copyTo(dst_xiao, mask_xiao);
		circle(mask_da, circleCenter, R2, Scalar(255),-1);//圆的半径可调节参数  略小一点好
		image_roi.copyTo(dst_da, mask_da);
		Mat dst = dst_da - dst_xiao;
		imshow("dst",dst);
		//----------------------------------------以上取圆环----------------------------------------------------------------
		int count=0;
		float bad_rate = 0;
		
		threshold( dst, dst, 200, 255, CV_THRESH_BINARY );
		imshow("erzhihua",dst);
		//-------------------------------------------------求破损比例-------------------------------------------------------
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		Canny(dst, dst, 300, 900, 3);
		findContours(dst, contours, hierarchy, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
		for (int i = 0; i < (int)contours.size(); i++)
		{
			Scalar color(rand() & 255, rand() & 255, rand() & 255);
			drawContours(dst, contours, i, Scalar(255), 1, 8,hierarchy);
		}
		//imshow("【处理后的图像】", dst);
		float endg_dConArea = 0;
		//计算轮廓的面积
		for (int i = 0; i < (int)contours.size(); i++)
		{
			double g_dConArea = contourArea(contours[i], true);
			//cout << "【用轮廓面积计算函数计算出来的第" << i << "个轮廓的面积为：】" << g_dConArea << endl;
			//--------------------------------------确定面积大小，半段瓶口破损与否----------和所占破损比例-------------------
			if(abs(g_dConArea)>10)   //15为可调价参数，修改破损所占面积大小
			{
				endg_dConArea =endg_dConArea+abs(g_dConArea);
			}
		}
			cout<<"轮廓面积为:"<<endg_dConArea<<endl;
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
		//-------------------------------------------------如果是非破损瓶口-------------------------------------------------------------
		Size axes;
		Mat gray;
		cvtColor(image_roi,gray,CV_BGR2GRAY);      //把image转换成灰度放到gray中
		Mat threshold_out;
		threshold( gray, threshold_out, 170, 255, CV_THRESH_BINARY );    //二值化结果影响椭圆检测 
		imshow("qqwqwq",threshold_out);
		//vector<vector<Point>> contours;
		//vector<Vec4i> hierarchy;
		float midel_radius = 0,large_radius = 0, small_radius = 0;
		findContours(threshold_out, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat cimage  = Mat::zeros(threshold_out.size(), CV_8UC3);
		for(size_t i = 0; i < contours.size(); i++)
		{
			//拟合的点至少为6
			size_t count = contours[i].size();
			if( count < 550 )   //去掉椭圆小于一定数量的圆,需调整
				continue;

			//椭圆拟合
			RotatedRect box = fitEllipse(contours[i]);
			center.x = cvRound(box.center.x);     /*确定椭圆box的值center、size、angle*/
			center.y = cvRound(box.center.y);

			axes.width = cvRound(box.size.width*0.5);
			axes.height = cvRound(box.size.height*0.5);
			box.angle = -box.angle;

			if(abs(axes.width-axes.height)<2)         //拟合多个圆，设定长短轴条件确定需要的圆2个，求这两个圆的长短轴并输出
			{
				drawContours(cimage , contours, (int)i, Scalar::all(255), 1, 8);
				ellipse(image_roi , box, Scalar(0,0,255), 1, CV_AA);
				imshow("拟合椭圆结果", image_roi );
				//printf("中心位置：x=%d,y=%d\n",axes.width,axes.height);
				midel_radius = (axes.width+axes.height)/2;
				cout<<"中间半径为 ："<<midel_radius<<endl;
				float A_area = PI*axes.width*axes.height;
				float A_length = 2*PI*axes.height+4*(abs(axes.width-axes.height));
				float  circular_degree = (4*PI*A_area)/(A_length*A_length);
				cout<<"圆度为："<<circular_degree<<endl;
				if(midel_radius<270)              //根据相机高度调节的参数，需要标定huantujiugai
				{
					small_radius= midel_radius;
				}
				else
				{
					large_radius= midel_radius;
				}

				cout<<"-------------------------------------区分两个椭圆参数-----------------------------------------\n"<<endl;
			}
		}
		//cout<<"small is :"<<small_radius<<"\n"<<"large is :"<<large_radius<<"\n"<<endl;
		//------------------------------------------------求内外径-------------------------------------------------------------
		float INNERstanderd_bottle_r = 10.5,OUTstanderd_bottle_r = 13.0;
		float UPDOWN_deflection = 0.2;

		float pixcel_area =  0.06; //标定
		float INNERCircle_radius =small_radius*pixcel_area;
		float OUTCircle_radius =large_radius*pixcel_area;
		cout<<"内圆的半径为 ："<<INNERCircle_radius<<endl;
		cout<<"外圆的半径为 ："<<OUTCircle_radius<<endl;

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