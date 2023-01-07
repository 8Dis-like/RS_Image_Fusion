#include <opencv2/opencv.hpp>
#include <iostream>
#include<time.h>

using namespace cv;
using namespace std;

//将RGB图像融合为彩色图像并对比度增强
Mat RGBfusion(string,string,string);

void Contraststretch(Mat& M);
void Contraststretch(Mat& M, int minout, int maxout);
void HPF(Mat& pan, Mat& color);
void weighing(Mat&, Mat&);

