#pragma once
#pragma once
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <iostream>
#include<math.h>
#include<Windows.h>
#include"dispose.h"
#define PI 3.1415926

using namespace cv;
using namespace std;
float r[216][297];
float g[216][297];
float b[216][297];
float I_I[216][297];
Mat RGBtoHIS_geo(Mat& Mr, Mat& Mg, Mat& Mb) {
	for (int i = 0; i < 216; i++) {
		for (int j = 0; j < 297; j++) {
			r[i][j] = (int)Mr.at<uchar>(i, j);
			g[i][j] = (int)Mg.at<uchar>(i, j);
			b[i][j] = (int)Mb.at<uchar>(i, j);
		}
	}//����r,g,b�����λҶ�ֵ
	Mat hsi(216, 297, CV_8UC3);
	float H = 0, I = 0, S = 0;
	for (int i = 0; i < 216; i++) {
		for (int j = 0; j < 297; j++) {
			float R = r[i][j] / 255.f;
			float G = g[i][j] / 255.f;
			float B = b[i][j] / 255.f;//��һ������
	
			float k = (R - G + R - B) / 2;
			float L = sqrt((R - G) * (R - G) + (R - B) * (G - B));
			if (L == 0) {
				H = 0;
			}
			else {
				float theta = acos(k / L);
				if (B <= G) {
					H = theta / (2 * PI);
				}
				else {
					H = 1 - theta / (2 * PI);
				}
			}
			float sum = R + G + B;
			if (sum == 0) {
				S = 0;
			}
			else {
				S = 1 - 3 * min(min(B, G), R) / sum;
			}
			I = sum / 3.0;
			I_I[i][j] = I * 255;
			hsi.at<Vec3b>(i, j)[0] = H * 255;
			hsi.at<Vec3b>(i, j)[1] = S * 255;
			hsi.at<Vec3b>(i, j)[2] = I * 255;
		}
	}//RGBתHSI���й�ʽ
	return hsi;
}
Mat RGBtoHIS_cylin(Mat& Mr, Mat& Mg, Mat& Mb) {
	for (int i = 0; i < 216; i++) {
		for (int j = 0; j < 297; j++) {
			r[i][j] = (int)Mr.at<uchar>(i, j);
			g[i][j] = (int)Mg.at<uchar>(i, j);
			b[i][j] = (int)Mb.at<uchar>(i, j);
		}
	}//����r,g,b�����λҶ�ֵ
	Mat hsi(216, 297, CV_8UC3);
	float H = 0, I = 0, S = 0;
	for (int i = 0; i < 216; i++) {
		for (int j = 0; j < 297; j++) {
			float R = r[i][j] / 255.f;
			float G = g[i][j] / 255.f;
			float B = b[i][j] / 255.f;//��һ������
			I = 1 / sqrt(3) * (R + G + B);
			if (G == B) {
				H = PI / 2;
			}
			else if (G > B) {
				H = atan((2 * R - G - B) / (sqrt(3) * (G - B)));
			}
			else {
				H = atan((2 * R - G - B) / (sqrt(3) * (G - B))) + PI;
			}
			S = (sqrt(6) / 3) * sqrt(R * R + G * G + B * B - R * G - R * B - G * B);
			
			I_I[i][j] = I * 255;
			hsi.at<Vec3b>(i, j)[0] = H * 255;
			hsi.at<Vec3b>(i, j)[1] = S * 255;
			hsi.at<Vec3b>(i, j)[2] = I * 255;
		}
	}//RGBתHSI���й�ʽ
	return hsi;
}
void HISchanged(string path_r, string path_g, string path_b, string path_pan) {
	Mat Mr = imread(path_r, IMREAD_GRAYSCALE);
	Mat Mg = imread(path_g, IMREAD_GRAYSCALE);
	Mat Mb = imread(path_b, IMREAD_GRAYSCALE);
	
	Mat M(216, 297, CV_8UC3, Scalar(0, 0, 255));//������R,G,B��������ɵ�ͼ��

	//for (int i = 0; i < 216; i++) {
	//	for (int j = 0; j < 297; j++) {
	//		M.at<Vec3b>(i, j)[0] = b[i][j];
	//		M.at<Vec3b>(i, j)[1] = g[i][j];
	//		M.at<Vec3b>(i, j)[2] = r[i][j];
	//	}
	//}
	//�޸İ汾
	vector<Mat> channels;
	channels.push_back(Mr);
	channels.push_back(Mg);
	channels.push_back(Mb);
	merge(channels, M);
	
	Mat hsi = RGBtoHIS_geo(Mr, Mg, Mb);//����HIS�任--���α任��
	//Mat hsi = RGBtoHIS_cylin(Mr, Mg, Mb); //Բ����任

	resize(M, M, { 594,430 }, 0, 0, cv::INTER_CUBIC);
	imshow("rgbimage", M);//ԭ��ɫͼ����resize�Ŵ��
	imshow("hsiimage", hsi);//hsi����ͼ��
	resize(hsi, hsi, { 594,430 }, 0, 0, cv::INTER_CUBIC);
	imshow("hsinewimage", hsi);//resize�Ŵ���hsi����ͼ��
	Mat Mpan = imread(path_pan, IMREAD_GRAYSCALE);//pan����
	Mat MI(216, 297, CV_8UC1);//I���ε�������
	for (int i = 0; i < 216; i++) {
		for (int j = 0; j < 297; j++) {
			MI.at<uchar>(i, j) = I_I[i][j];
		}
	}
	Mat MInew = Match(Mpan, MI);//pan������I����Ϊ�ο�������ֱ��ͼƥ��
	Mat rgb(430, 594, CV_8UC3);
	for (int i = 0; i < 430; i++) {
		for (int j = 0; j < 594; j++) {
			hsi.at<Vec3b>(i, j)[2] = (float)MInew.at<uchar>(i, j);
		}
	}//��ƥ����MInew�滻I����
	imshow("hsinewnewimage", hsi);
	float  R = 0, G = 0, B = 0;
	for (int i = 0; i < 430; i++) {
		for (int j = 0; j < 594; j++) {
			float H = hsi.at<Vec3b>(i, j)[0] / 255.f * 2 * PI;
			float S = hsi.at<Vec3b>(i, j)[1] / 255.f;
			float I = hsi.at<Vec3b>(i, j)[2] / 255.f;
			if (H < 120.f * PI / 180.f) {
				B = I * (1 - S);
				R = I * (1 + S * cos(H) / cos(60.f * PI / 180.f - H));
				G = 3 * I - R - B;
			}
			else if (H >= 120.f * PI / 180.f && H < 240 * PI / 180.f) {
				H -= (120.f * PI / 180.f);
				R = I * (1 - S);
				G = I * (1 + S * cos(H) / cos(60.f * PI / 180.f - H));
				B = 3 * I - R - G;
			}
			else if (H >= 240.f * PI / 180.f) {
				H -= (240.f * PI / 180.f);
				G = I * (1 - S);
				B = I * (1 + S * cos(H) / cos(60.f * PI / 180.f - H));
				R = 3 * I - B - G;
			}

			rgb.at<Vec3b>(i, j)[0] = B * 255;
			rgb.at<Vec3b>(i, j)[1] = G * 255;
			rgb.at<Vec3b>(i, j)[2] = R * 255;
		}
	}//HSIתRGB
	imshow("merge_result", rgb);//���ս��
}
